#pragma once

#include "ArduSubFirmwarePlugin.h"

/// ArduPilot Sub, as AETHORAX plans for it: the mission dropdown is set by
/// aethorax.json (AethoraxConfig): ArduSub's commands and AETHORAX's operations.
/// Their names, fields and limits are in the base command list (AethoraxBaseFirmwarePlugin).
class AethoraxSubFirmwarePlugin : public ArduSubFirmwarePlugin
{
    Q_OBJECT

public:
    explicit AethoraxSubFirmwarePlugin(QObject *parent = nullptr);

    QList<MAV_CMD> supportedMissionCommands(QGCMAVLink::VehicleClass_t vehicleClass) const override;
};
