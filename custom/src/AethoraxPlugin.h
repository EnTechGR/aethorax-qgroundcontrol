#pragma once

#include "QGCCorePlugin.h"

#include <QtCore/QVariantMap>
#include <QtCore/QJsonObject>
#include <QtQml/QQmlAbstractUrlInterceptor>

/// Loads qrc:/Custom/<path> instead of qrc:/<path> when custom.qrc provides one: how
/// AETHORAX replaces an upstream QML file without changing it (custom/qml/).
class QTranslator;

class AethoraxQmlOverrides : public QQmlAbstractUrlInterceptor
{
public:
    QUrl intercept(const QUrl &url, QQmlAbstractUrlInterceptor::DataType type) final;
};

/// AETHORAX's core plugin: planning defaults for one vehicle, Nemo1 (ArduPilot Sub).
class AethoraxPlugin : public QGCCorePlugin
{
    Q_OBJECT

public:
    explicit AethoraxPlugin(QObject *parent = nullptr);

    static QGCCorePlugin *instance();

    /// Plans made without a vehicle connected are for ArduPilot Sub; the choice is not offered.
    void adjustSettingMetaData(const QString &settingsGroup, FactMetaData &metaData, bool &userVisible) final;

    QString stableDownloadLocation() const final { return QStringLiteral("github.com/EnTechGR/AETHORAX/releases"); }

    /// The plan file's "aethorax" section (AethoraxMissionStart)
    void postSaveToJson(PlanMasterController *pController, QJsonObject &json) final;
    void postLoadFromJson(PlanMasterController *pController, QJsonObject &json) final;

    QQmlApplicationEngine *createQmlApplicationEngine(QObject *parent) final;
    void destroyQmlApplicationEngine(QQmlApplicationEngine *qmlEngine) final;

    /// For the command picker (qml/MissionCommandDialog.qml): aethorax.json's categoriesFirst.
    Q_INVOKABLE QStringList categoriesFirst() const;

    /// For QML that AETHORAX overrides: a section of aethorax.json by dotted path
    /// (e.g. "planView.rightPanel"); see AethoraxConfig::section.
    Q_INVOKABLE QVariantMap config(const QString &section) const;

    /// A command's number from its rawName (e.g. "MAV_CMD_USER_2"); 0 if the commands file has none.
    Q_INVOKABLE int commandNumber(const QString &rawName) const;

    /// For qml/MissionSettingsEditor.qml: the devices' status at the mission's start (AethoraxMissionStart).
    Q_INVOKABLE QVariantMap initialStatus(QObject *settingsItem) const;
    Q_INVOKABLE void setInitialStatus(QObject *settingsItem, const QString &device, bool on);

private:
    QQmlApplicationEngine *_qmlEngine = nullptr;
    AethoraxQmlOverrides *_qmlOverrides = nullptr;
    QTranslator *_labels = nullptr;
};
