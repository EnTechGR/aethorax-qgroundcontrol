#include "AethoraxConfig.h"

#include "AppMessages.h"
#include "QGCLoggingCategory.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QHash>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QStandardPaths>

QGC_LOGGING_CATEGORY(AethoraxConfigLog, "Aethorax.Config")

namespace {

constexpr const char *kEnvironmentVariable = "AETHORAX_QGC_CONFIG";
constexpr const char *kBuiltInDirectory = ":/aethorax/config";
constexpr const char *kManifestFile = "aethorax.json";
constexpr const char *kManifestFileType = "AethoraxConfig";
constexpr const char *kCommandsFileType = "MavCmdInfo";

/// The JSON object in a file, if it parses and has the expected fileType and a supported version.
bool readJsonObject(const QString &path, const QString &fileType, QJsonObject &object, QString &error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        error = file.errorString();
        return false;
    }
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        error = QStringLiteral("%1 at offset %2").arg(parseError.errorString()).arg(parseError.offset);
        return false;
    }
    if (!document.isObject()) {
        error = QStringLiteral("the root is not an object");
        return false;
    }
    object = document.object();
    if (object.value(QStringLiteral("fileType")).toString() != fileType) {
        error = QStringLiteral("fileType is not \"%1\"").arg(fileType);
        return false;
    }
    if (object.value(QStringLiteral("version")).toInt() != 1) {
        error = QStringLiteral("version is not 1");
        return false;
    }
    return true;
}

void reportInvalid(const QString &path, const QString &error)
{
    const QString message = QStringLiteral("AETHORAX: %1 is not used: %2").arg(QDir::toNativeSeparators(path), error);
    qCWarning(AethoraxConfigLog) << message;
    QGC::showAppMessage(message);
}

/// The first valid copy of a file, in search order. The built-in copy is always valid.
QString findFile(const QString &name, const QString &fileType, QJsonObject &object)
{
    for (const QString &directory : AethoraxConfig::searchDirectories()) {
        const QString path = QDir(directory).filePath(name);
        if (!QFileInfo::exists(path)) {
            continue;
        }
        QString error;
        if (readJsonObject(path, fileType, object, error)) {
            qCDebug(AethoraxConfigLog) << "Using" << path;
            return path;
        }
        reportInvalid(path, error);
    }
    return QString();
}

/// Everything read from the files, once.
struct Loaded
{
    QJsonObject manifest;
    QString commandsFile;
    QHash<QString, int> commandIds;  ///< rawName → id, from the commands file
    bool keepFirmwareCommands = true;
    QJsonArray add;
    QJsonArray remove;
    QStringList categoriesFirst;
};

const Loaded &loaded()
{
    static const Loaded result = [] {
        Loaded l;
        QJsonObject manifest;
        if (findFile(QString::fromLatin1(kManifestFile), QString::fromLatin1(kManifestFileType), manifest).isEmpty()) {
            qCWarning(AethoraxConfigLog) << "No valid" << kManifestFile;
            return l;
        }
        l.manifest = manifest;
        const QJsonObject missionCommands = manifest.value(QStringLiteral("missionCommands")).toObject();
        const QJsonObject dropdown = missionCommands.value(QStringLiteral("dropdown")).toObject();
        l.keepFirmwareCommands = dropdown.value(QStringLiteral("firmwareCommands")).toBool(true);
        l.add = dropdown.value(QStringLiteral("add")).toArray();
        l.remove = dropdown.value(QStringLiteral("remove")).toArray();
        for (const QJsonValue &category : dropdown.value(QStringLiteral("categoriesFirst")).toArray()) {
            l.categoriesFirst << category.toString();
        }

        const QString commandsName = missionCommands.value(QStringLiteral("file")).toString(QStringLiteral("mission_commands.json"));
        QJsonObject commands;
        l.commandsFile = findFile(commandsName, QString::fromLatin1(kCommandsFileType), commands);
        for (const QJsonValue &entry : commands.value(QStringLiteral("mavCmdInfo")).toArray()) {
            const QJsonObject info = entry.toObject();
            l.commandIds.insert(info.value(QStringLiteral("rawName")).toString(), info.value(QStringLiteral("id")).toInt());
        }
        return l;
    }();
    return result;
}

/// A dropdown entry: a command number, or a rawName from the commands file.
bool commandId(const QJsonValue &value, MAV_CMD &command)
{
    if (value.isDouble()) {
        command = static_cast<MAV_CMD>(value.toInt());
        return true;
    }
    const auto found = loaded().commandIds.constFind(value.toString());
    if (found == loaded().commandIds.constEnd()) {
        qCWarning(AethoraxConfigLog) << "Dropdown entry" << value << "is not in" << loaded().commandsFile;
        return false;
    }
    command = static_cast<MAV_CMD>(found.value());
    return true;
}

}  // namespace

QStringList AethoraxConfig::searchDirectories()
{
    QStringList directories;
    const QString fromEnvironment = qEnvironmentVariable(kEnvironmentVariable);
    if (!fromEnvironment.isEmpty()) {
        directories << fromEnvironment;
    }
    const QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (!documents.isEmpty()) {
        directories << QDir(documents).filePath(QCoreApplication::applicationName() + QStringLiteral("/Config"));
    }
    directories << QString::fromLatin1(kBuiltInDirectory);
    return directories;
}

QString AethoraxConfig::missionCommandsFile()
{
    return loaded().commandsFile;
}

QList<MAV_CMD> AethoraxConfig::missionDropdown(const QList<MAV_CMD> &firmwareCommands)
{
    QList<MAV_CMD> commands = loaded().keepFirmwareCommands ? firmwareCommands : QList<MAV_CMD>();
    MAV_CMD command;
    for (const QJsonValue &value : loaded().add) {
        if (commandId(value, command) && !commands.contains(command)) {
            commands.append(command);
        }
    }
    for (const QJsonValue &value : loaded().remove) {
        if (commandId(value, command)) {
            commands.removeAll(command);
        }
    }
    return commands;
}

QStringList AethoraxConfig::categoriesFirst()
{
    return loaded().categoriesFirst;
}

int AethoraxConfig::commandNumber(const QString &rawName)
{
    return loaded().commandIds.value(rawName, 0);
}

QVariantMap AethoraxConfig::section(const QString &path)
{
    QJsonObject object = loaded().manifest;
    for (const QString &key : path.split(QLatin1Char('.'), Qt::SkipEmptyParts)) {
        object = object.value(key).toObject();
    }
    return object.toVariantMap();
}
