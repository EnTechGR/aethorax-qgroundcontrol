#include "AethoraxSubFirmwarePlugin.h"

AethoraxSubFirmwarePlugin::AethoraxSubFirmwarePlugin(QObject *parent)
    : ArduSubFirmwarePlugin(parent)
{
}

QList<MAV_CMD> AethoraxSubFirmwarePlugin::supportedMissionCommands(QGCMAVLink::VehicleClass_t /*vehicleClass*/) const
{
    // The AETHORAX operations (MAVLink user commands, one per operation) and the
    // commands the Jetson's plan tool still maps until their own operation exists.
    // The plan goes to the Jetson as a file through the operator console; ArduSub
    // never stores these items.
    return {
        MAV_CMD_USER_1,         // Dive
        MAV_CMD_NAV_WAYPOINT,   // Go to a position
        MAV_CMD_DO_CHANGE_SPEED,
        MAV_CMD_DO_SET_SERVO,   // Lights
    };
}

QString AethoraxSubFirmwarePlugin::missionCommandOverrides(QGCMAVLink::VehicleClass_t vehicleClass) const
{
    if (vehicleClass == QGCMAVLink::VehicleClassSub) {
        return QStringLiteral(":/aethorax/AETHORAX-MavCmdInfoSub.json");
    }
    return ArduSubFirmwarePlugin::missionCommandOverrides(vehicleClass);
}
