#include "AethoraxPlugin.h"

#include "AppSettings.h"
#include "FactMetaData.h"
#include "QGCMAVLink.h"

#include <QtCore/QApplicationStatic>

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
