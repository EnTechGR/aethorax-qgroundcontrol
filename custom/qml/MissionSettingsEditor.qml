import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls
import QGroundControl.PlanView

// AETHORAX: upstream src/PlanView/MissionSettingsEditor.qml (v5.1.5), loaded instead of it
// (AethoraxQmlOverrides). The only change: the devices' status at the mission's start, on or
// off, above the camera settings; the devices come from aethorax.json's missionStart.devices
// and the status is saved in the .plan file (AethoraxMissionStart). Search for "AETHORAX".
// After merging an upstream release, diff against the upstream file and carry its changes over.
// Editor for Initial Camera Settings (mission item 0)
Rectangle {
    id: root
    width: availableWidth
    height: valuesColumn.height + (_margin * 2)
    color: qgcPal.windowShadeDark
    radius: ScreenTools.defaultBorderRadius

    required property var missionItem
    required property real availableWidth

    property var _masterController: missionItem.masterController
    property var _controllerVehicle: _masterController.controllerVehicle
    property bool _waypointsOnlyMode: QGroundControl.corePlugin.options.missionWaypointsOnly
    property bool _showCameraSection: _waypointsOnlyMode || QGroundControl.corePlugin.showAdvancedUI
    property bool _simpleMissionStart: QGroundControl.corePlugin.options.showSimpleMissionStart

    readonly property real _margin: ScreenTools.defaultFontPixelWidth / 2

    // AETHORAX: the devices, and their status in this plan
    readonly property var _devices: QGroundControl.corePlugin.config("missionStart").devices || []
    property var _initialStatus: QGroundControl.corePlugin.initialStatus(missionItem)

    QGCPalette { id: qgcPal }

    ColumnLayout {
        id: valuesColumn
        anchors.margins: _margin
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        spacing: ScreenTools.defaultFontPixelHeight / 2

        // AETHORAX: the devices' status at the mission's start
        SectionHeader {
            id: initialStatusHeader
            Layout.fillWidth: true
            text: qsTr("Initial Status")
            visible: _devices.length > 0
        }

        Repeater {
            model: initialStatusHeader.visible && initialStatusHeader.checked ? _devices : []

            RowLayout {
                Layout.fillWidth: true

                QGCLabel {
                    Layout.fillWidth: true
                    text: modelData.label || modelData.key
                    elide: Text.ElideRight
                }

                QGCLabel {
                    text: deviceSwitch.checked ? qsTr("On") : qsTr("Off")
                }

                Switch {
                    id: deviceSwitch
                    objectName: "missionStart_" + modelData.key
                    checked: root._initialStatus[modelData.key] === true
                    onToggled: {
                        QGroundControl.corePlugin.setInitialStatus(root.missionItem, modelData.key, checked)
                        root._initialStatus = QGroundControl.corePlugin.initialStatus(root.missionItem)
                    }
                }
            }
        }

        Column {
            Layout.fillWidth: true
            spacing: _margin
            visible: !_simpleMissionStart

            CameraSection {
                id: cameraSection
                anchors.left: parent.left
                anchors.right: parent.right
                missionItem: root.missionItem
                visible: _showCameraSection
                showSectionHeader: false

                Component.onCompleted: checked = !_waypointsOnlyMode && missionItem.cameraSection.settingsSpecified
            }

            QGCLabel {
                anchors.left: parent.left
                anchors.right: parent.right
                text: qsTr("Above camera commands will take affect immediately upon mission start.")
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: ScreenTools.smallFontPointSize
                visible: _showCameraSection && cameraSection.checked
            }
        }
    }
}
