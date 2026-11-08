#include "AethoraxPlugin.h"
#include "AethoraxConfig.h"
#include "AethoraxLabels.h"
#include "AethoraxMissionStart.h"

#include "AppSettings.h"
#include "FactMetaData.h"
#include "QGCMAVLink.h"

#include <QtCore/QApplicationStatic>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtQml/QQmlApplicationEngine>

Q_APPLICATION_STATIC(AethoraxPlugin, _aethoraxPluginInstance);

AethoraxPlugin::AethoraxPlugin(QObject *parent)
    : QGCCorePlugin(parent)
{
}

QGCCorePlugin *AethoraxPlugin::instance()
{
    return _aethoraxPluginInstance();
}

void AethoraxPlugin::adjustSettingMetaData(const QString &settingsGroup, FactMetaData &metaData, bool &userVisible)
{
    QGCCorePlugin::adjustSettingMetaData(settingsGroup, metaData, userVisible);

    if (settingsGroup == AppSettings::settingsGroup) {
        if (metaData.name() == AppSettings::offlineEditingFirmwareClassName) {
            metaData.setRawDefaultValue(QGCMAVLink::FirmwareClassArduPilot);
            userVisible = false;
        } else if (metaData.name() == AppSettings::offlineEditingVehicleClassName) {
            metaData.setRawDefaultValue(QGCMAVLink::VehicleClassSub);
            userVisible = false;
        }
    }
}

QQmlApplicationEngine *AethoraxPlugin::createQmlApplicationEngine(QObject *parent)
{
    if (!_labels) {
        _labels = new AethoraxLabels(this);
        QCoreApplication::installTranslator(_labels);
    }
    _qmlEngine = QGCCorePlugin::createQmlApplicationEngine(parent);
    _qmlOverrides = new AethoraxQmlOverrides();
    _qmlEngine->addUrlInterceptor(_qmlOverrides);
    return _qmlEngine;
}

void AethoraxPlugin::destroyQmlApplicationEngine(QQmlApplicationEngine *qmlEngine)
{
    if (qmlEngine && (qmlEngine == _qmlEngine)) {
        qmlEngine->removeUrlInterceptor(_qmlOverrides);
        delete _qmlOverrides;
        _qmlOverrides = nullptr;
        _qmlEngine = nullptr;
    }
    QGCCorePlugin::destroyQmlApplicationEngine(qmlEngine);
}

QStringList AethoraxPlugin::categoriesFirst() const
{
    return AethoraxConfig::categoriesFirst();
}

QVariantMap AethoraxPlugin::config(const QString &section) const
{
    return AethoraxConfig::section(section);
}

int AethoraxPlugin::commandNumber(const QString &rawName) const
{
    return AethoraxConfig::commandNumber(rawName);
}

QVariantMap AethoraxPlugin::initialStatus(QObject *settingsItem) const
{
    return AethoraxMissionStart::initialStatus(settingsItem);
}

void AethoraxPlugin::setInitialStatus(QObject *settingsItem, const QString &device, bool on)
{
    AethoraxMissionStart::setInitialStatus(settingsItem, device, on);
}

void AethoraxPlugin::postSaveToJson(PlanMasterController *pController, QJsonObject &json)
{
    QGCCorePlugin::postSaveToJson(pController, json);
    AethoraxMissionStart::save(pController, json);
}

void AethoraxPlugin::postLoadFromJson(PlanMasterController *pController, QJsonObject &json)
{
    QGCCorePlugin::postLoadFromJson(pController, json);
    AethoraxMissionStart::load(pController, json);
}

QUrl AethoraxQmlOverrides::intercept(const QUrl &url, QQmlAbstractUrlInterceptor::DataType type)
{
    if ((type == QQmlAbstractUrlInterceptor::QmlFile || type == QQmlAbstractUrlInterceptor::UrlString)
        && url.scheme() == QStringLiteral("qrc")) {
        const QString overridePath = QStringLiteral("/Custom") + url.path();
        if (QFile::exists(QStringLiteral(":") + overridePath)) {
            QUrl result;
            result.setScheme(QStringLiteral("qrc"));
            result.setPath(overridePath);
            return result;
        }
    }
    return url;
}
