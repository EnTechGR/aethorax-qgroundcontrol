#pragma once

#include "ArduSubFirmwarePlugin.h"

/// ArduPilot Sub, as AETHORAX plans for it: the mission dropdown offers only
/// AETHORAX's operations, each with its own fields and limits
/// (res/AETHORAX-MavCmdInfoSub.json, generated from the AETHORAX config by
/// tools/generate_mission_commands.py).
class AethoraxSubFirmwarePlugin : public ArduSubFirmwarePlugin
{
    Q_OBJECT

public:
    explicit AethoraxSubFirmwarePlugin(QObject *parent = nullptr);

    QList<MAV_CMD> supportedMissionCommands(QGCMAVLink::VehicleClass_t vehicleClass) const override;
    QString missionCommandOverrides(QGCMAVLink::VehicleClass_t vehicleClass) const override;
};
