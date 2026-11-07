#include "AethoraxPlugin.h"
#include "AethoraxConfig.h"

#include "AppSettings.h"
#include "FactMetaData.h"
#include "QGCMAVLink.h"

#include <QtCore/QApplicationStatic>
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
