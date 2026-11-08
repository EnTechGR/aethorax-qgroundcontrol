#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

class PlanMasterController;
class QObject;

/// The devices' status at the mission's start (camera, DVL, sonar...): on or off, set in the
/// first mission item's editor (qml/MissionSettingsEditor.qml) and saved in the .plan file as
///   "aethorax": {"version": 1, "initialStatus": {"<device key>": true | false, ...}}
/// The devices, their names and defaults are aethorax.json's missionStart.devices.
namespace AethoraxMissionStart
{
    /// Every device's status for a plan's first item (MissionSettingsItem): its own, or the default.
    QVariantMap initialStatus(QObject *settingsItem);

    /// Sets one device's status, and marks the plan as changed.
    void setInitialStatus(QObject *settingsItem, const QString &device, bool on);

    /// The plan file's "aethorax" section, written on save and read on load.
    void save(PlanMasterController *plan, QJsonObject &planJson);
    void load(PlanMasterController *plan, const QJsonObject &planJson);
}
