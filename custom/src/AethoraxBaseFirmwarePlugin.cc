#include "AethoraxBaseFirmwarePlugin.h"
#include "AethoraxConfig.h"

AethoraxBaseFirmwarePlugin::AethoraxBaseFirmwarePlugin(QObject *parent)
    : FirmwarePlugin(parent)
{
}

QString AethoraxBaseFirmwarePlugin::missionCommandOverrides(QGCMAVLink::VehicleClass_t vehicleClass) const
{
    if (vehicleClass == QGCMAVLink::VehicleClassGeneric) {
        return AethoraxConfig::missionCommandsFile();
    }
    return FirmwarePlugin::missionCommandOverrides(vehicleClass);
}
