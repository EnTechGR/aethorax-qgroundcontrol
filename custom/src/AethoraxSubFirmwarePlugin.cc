#include "AethoraxSubFirmwarePlugin.h"
#include "AethoraxConfig.h"

AethoraxSubFirmwarePlugin::AethoraxSubFirmwarePlugin(QObject *parent)
    : ArduSubFirmwarePlugin(parent)
{
}

QList<MAV_CMD> AethoraxSubFirmwarePlugin::supportedMissionCommands(QGCMAVLink::VehicleClass_t vehicleClass) const
{
    // ArduSub's own commands and AETHORAX's operations, as aethorax.json says
    // (AethoraxConfig). A plan with operations goes to the Jetson as a file through
    // the operator console: ArduSub doesn't store user commands.
    return AethoraxConfig::missionDropdown(ArduSubFirmwarePlugin::supportedMissionCommands(vehicleClass));
}
