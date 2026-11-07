#include "AethoraxBaseFirmwarePlugin.h"

AethoraxBaseFirmwarePlugin::AethoraxBaseFirmwarePlugin(QObject *parent)
    : FirmwarePlugin(parent)
{
}

QString AethoraxBaseFirmwarePlugin::missionCommandOverrides(QGCMAVLink::VehicleClass_t vehicleClass) const
{
    if (vehicleClass == QGCMAVLink::VehicleClassGeneric) {
        return QStringLiteral(":/aethorax/AETHORAX-MavCmdInfoCommon.json");
    }
    return FirmwarePlugin::missionCommandOverrides(vehicleClass);
}
