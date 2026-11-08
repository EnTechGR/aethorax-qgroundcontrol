import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.PlanView

// AETHORAX: upstream src/PlanView/PlanViewRightPanel.qml (v5.1.5), loaded instead of it
// (AethoraxQmlOverrides). The only change: the panel's left edge can be dragged to make it
// wider or narrower; the width is kept between runs, and a double-click on the edge resets it.
// The limits, the edge and its cursor come from aethorax.json, planView.rightPanel; the
// defaults below are used for anything it doesn't set.
// After merging an upstream release, diff against the upstream file and carry its changes over.
Item {
    required property var editorMap
    required property var planMasterController

    signal editingLayerChangeRequested(int layer)

    id: root

    property var  _missionController: planMasterController.missionController
    property real _toolsMargin:       ScreenTools.defaultFontPixelWidth * 0.75

    // aethorax.json, planView.rightPanel
    readonly property var  _config:       QGroundControl.corePlugin.config("planView.rightPanel")
    readonly property var  _edgeConfig:   _config.edge || {}
    readonly property var  _gripConfig:   _edgeConfig.grip || {}
    readonly property bool _resizable:    _config.resizable ?? true

    // The width: upstream's until the edge is dragged, then the dragged one, kept within limits
    property real _defaultWidth:  parent ? Math.min(parent.width / 3, ScreenTools.defaultFontPixelWidth * 30) : 0
    property real _minimumWidth:  Math.min(_defaultWidth, ScreenTools.defaultFontPixelWidth * (_config.minimumWidth ?? 20))
    property real _maximumWidth:  parent ? Math.max(_minimumWidth, parent.width * (_config.maximumWidth ?? 0.75)) : 0
    property real _draggedWidth:  0

    Settings {
        id:         savedWidth
        category:   "AethoraxPlanView"

        property real rightPanelWidth: 0
    }

    Component.onCompleted: {
        _draggedWidth = savedWidth.rightPanelWidth
        width = Qt.binding(function() {
            return _resizable && _draggedWidth > 0 ? Math.max(_minimumWidth, Math.min(_draggedWidth, _maximumWidth)) : _defaultWidth
        })
    }

    function selectNextNotReady() {
        for (var i = 0; i < _missionController.visualItems.count; i++) {
            var vmi = _missionController.visualItems.get(i)
            if (vmi.readyForSaveState === VisualMissionItem.NotReadyForSaveData) {
                _missionController.setCurrentPlanViewSeqNum(vmi.sequenceNumber, true)
                break
            }
        }
    }

    QGCPalette { id: qgcPal }

    Rectangle {
        id:             rightPanelBackground
        anchors.fill:   parent
        color:          qgcPal.window
        opacity:        0.85
    }


    // Open/Close panel
    Item {
        id:                     panelOpenCloseButton
        anchors.right:          parent.left
        anchors.verticalCenter: parent.verticalCenter
        width:                  toggleButtonRect.width - toggleButtonRect.radius
        height:                 toggleButtonRect.height
        clip:                   true

        property bool _expanded: root.anchors.right == root.parent.right

        Rectangle {
            id:             toggleButtonRect
            width:          ScreenTools.defaultFontPixelWidth * 2.25
            height:         width * 3
            radius:         ScreenTools.defaultBorderRadius
            color:          rightPanelBackground.color
            opacity:        rightPanelBackground.opacity

            QGCLabel {
                id:                 toggleButtonLabel
                anchors.centerIn:   parent
                text:               panelOpenCloseButton._expanded ? ">" : "<"
                color:              qgcPal.buttonText
            }

        }

        QGCMouseArea {
            anchors.fill: parent

            onClicked: {
                if (panelOpenCloseButton._expanded) {
                    // Close panel
                    root.anchors.right = undefined
                    root.anchors.left = root.parent.right
                } else {
                    // Open panel
                    root.anchors.left = undefined
                    root.anchors.right = root.parent.right
                }
            }
        }
    }

    //-------------------------------------------------------
    // Right Panel Controls
    Item {
        anchors.fill: rightPanelBackground

        DeadMouseArea {
            anchors.fill:   parent
        }

        PlanTreeView {
            id:                     planTreeView
            objectName:             "planView_planTree"
            anchors.fill:           parent
            editorMap:              root.editorMap
            planMasterController:   root.planMasterController
            onEditingLayerChangeRequested: (layer) => root.editingLayerChangeRequested(layer)
        }
    }

    // Drag the left edge to resize; double-click it for the default width
    Item {
        id:             resizeEdge
        anchors.left:   parent.left
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        width:          ScreenTools.defaultFontPixelWidth * (root._edgeConfig.width ?? 0.75)
        visible:        root._resizable

        Rectangle {
            anchors.horizontalCenter:   parent.horizontalCenter
            anchors.verticalCenter:     parent.verticalCenter
            width:                      Math.max(2, ScreenTools.defaultFontPixelWidth * (root._gripConfig.thickness ?? 0.2))
            height:                     ScreenTools.defaultFontPixelHeight * (root._gripConfig.length ?? 3)
            radius:                     width / 2
            color:                      qgcPal.buttonText
            opacity:                    resizeMouseArea.containsMouse || resizeMouseArea.pressed
                                        ? (root._gripConfig.hoverOpacity ?? 0.9) : (root._gripConfig.opacity ?? 0.35)
        }

        MouseArea {
            id:                 resizeMouseArea
            anchors.fill:       parent
            hoverEnabled:       true
            cursorShape:        Qt[root._edgeConfig.cursor] ?? Qt.SizeHorCursor
            preventStealing:    true

            property real _startX:      0
            property real _startWidth:  0

            onPressed: (mouse) => {
                _startX = mapToItem(root.parent, mouse.x, 0).x
                _startWidth = root.width
            }
            onPositionChanged: (mouse) => {
                if (pressed) {
                    const x = mapToItem(root.parent, mouse.x, 0).x
                    root._draggedWidth = Math.max(root._minimumWidth, Math.min(_startWidth + _startX - x, root._maximumWidth))
                }
            }
            onReleased: savedWidth.rightPanelWidth = root._draggedWidth
            onDoubleClicked: {
                root._draggedWidth = 0
                savedWidth.rightPanelWidth = 0
            }
        }
    }

    function selectLayer(nodeType) {
        // Ensure panel is open
        if (!panelOpenCloseButton._expanded) {
            root.anchors.left = undefined
            root.anchors.right = root.parent.right
        }
        planTreeView.selectLayer(nodeType)
    }
}
