#pragma once

#include "FirmwarePluginFactory.h"
#include "QGCMAVLink.h"

class AethoraxBaseFirmwarePlugin;
class AethoraxSubFirmwarePlugin;

/// The only firmware this build supports: ArduPilot Sub (Nemo1). Registered in
/// place of the stock ArduPilot and PX4 factories (cmake/CustomOverrides.cmake).
/// It also serves the generic firmware class, whose plugin provides the base
/// list of mission commands, so the AETHORAX operations can be named there.
class AethoraxFirmwarePluginFactory : public FirmwarePluginFactory
{
    Q_OBJECT

public:
    explicit AethoraxFirmwarePluginFactory(QObject *parent = nullptr);

    QList<QGCMAVLink::FirmwareClass_t> supportedFirmwareClasses() const final;
    QList<QGCMAVLink::VehicleClass_t> supportedVehicleClasses() const final;
    FirmwarePlugin *firmwarePluginForAutopilot(MAV_AUTOPILOT autopilotType, MAV_TYPE vehicleType) final;

private:
    AethoraxSubFirmwarePlugin *_subPlugin = nullptr;
    AethoraxBaseFirmwarePlugin *_basePlugin = nullptr;
};
