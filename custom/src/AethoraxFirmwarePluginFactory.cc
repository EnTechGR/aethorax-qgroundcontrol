#include "AethoraxFirmwarePluginFactory.h"

#include "AethoraxSubFirmwarePlugin.h"

// Registers itself with the FirmwarePluginFactoryRegister when constructed.
AethoraxFirmwarePluginFactory aethoraxFirmwarePluginFactory;

AethoraxFirmwarePluginFactory::AethoraxFirmwarePluginFactory(QObject *parent)
    : FirmwarePluginFactory(parent)
{
}

QList<QGCMAVLink::FirmwareClass_t> AethoraxFirmwarePluginFactory::supportedFirmwareClasses() const
{
    return { QGCMAVLink::FirmwareClassArduPilot };
}

QList<QGCMAVLink::VehicleClass_t> AethoraxFirmwarePluginFactory::supportedVehicleClasses() const
{
    return { QGCMAVLink::VehicleClassSub };
}

FirmwarePlugin *AethoraxFirmwarePluginFactory::firmwarePluginForAutopilot(MAV_AUTOPILOT autopilotType, MAV_TYPE /*vehicleType*/)
{
    // Any ArduPilot vehicle is treated as the Sub: this build plans for Nemo1 only.
    if (autopilotType != MAV_AUTOPILOT_ARDUPILOTMEGA) {
        return nullptr;
    }
    if (!_subPlugin) {
        _subPlugin = new AethoraxSubFirmwarePlugin(this);
    }
    return _subPlugin;
}
