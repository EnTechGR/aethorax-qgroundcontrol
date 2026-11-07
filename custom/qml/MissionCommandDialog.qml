import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import QGroundControl
import QGroundControl.Controls

// AETHORAX: upstream src/QmlControls/MissionCommandDialog.qml (v5.1.5), loaded instead of it
// (AethoraxQmlOverrides). The only change: the categories in aethorax.json's
// dropdown.categoriesFirst come first, in the category list and in "All commands".
// After merging an upstream release, diff against the upstream file and carry its changes over.
QGCPopupDialog {
    id:         root
    title:      qsTr("Select Mission Command")
    buttons:    Dialog.Cancel

    property var    vehicle
    property var    missionItem
    property var    map
    property bool   flyThroughCommandsAllowed

    property var    _firstCategories: QGroundControl.corePlugin.categoriesFirst()

    function _rank(category) {
        var index = _firstCategories.indexOf(category)
        return index < 0 ? _firstCategories.length : index
    }

    // Stable: keeps upstream's order within a rank
    function _firstCategoriesFirst(list, categoryOf) {
        var ranked = []
        for (var i = 0; i < list.length; i++) {
            ranked.push({ item: list[i], rank: _rank(categoryOf(list[i])), index: i })
        }
        ranked.sort(function(a, b) { return a.rank - b.rank || a.index - b.index })
        return ranked.map(function(entry) { return entry.item })
    }

    ColumnLayout {
        RowLayout {
            spacing: ScreenTools.defaultFontPixelWidth

            QGCLabel {
                text: qsTr("Category:")
            }

            QGCComboBox {
                id:                     categoryCombo
                Layout.preferredWidth:  30 * ScreenTools.defaultFontPixelWidth
                model:                  _firstCategoriesFirst(QGroundControl.missionCommandTree.categoriesForVehicle(vehicle),
                                                              function(category) { return category })

                function categorySelected(category) {
                    commandList.model = _firstCategoriesFirst(QGroundControl.missionCommandTree.getCommandsForCategory(vehicle, category, flyThroughCommandsAllowed),
                                                              function(info) { return info.category })
                }

                Component.onCompleted: {
                    var category  = missionItem.category
                    currentIndex = find(category)
                    categorySelected(category)
                }

                onActivated: (index) => { categorySelected(textAt(index)) }
            }
        }

        Repeater {
            id:                 commandList
            Layout.fillWidth:   true

            delegate: Rectangle {
                width:  parent.width
                height: commandColumn.height + ScreenTools.defaultFontPixelHeight
                color:  QGroundControl.globalPalette.button

                property var    mavCmdInfo: modelData
                property color  textColor:  QGroundControl.globalPalette.buttonText

                Column {
                    id:                 commandColumn
                    anchors.margins:    ScreenTools.defaultFontPixelWidth
                    anchors.left:       parent.left
                    anchors.right:      parent.right
                    anchors.top:        parent.top

                    QGCLabel {
                        text:           mavCmdInfo.friendlyName
                        color:          textColor
                        font.bold:      true
                    }

                    QGCLabel {
                        anchors.margins:    ScreenTools.defaultFontPixelWidth
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        text:               mavCmdInfo.description
                        wrapMode:           Text.WordWrap
                        color:              textColor
                    }
                }

                MouseArea {
                    anchors.fill:   parent
                    onClicked: {
                        missionItem.setMapCenterHintForCommandChange(map.center)
                        missionItem.command = mavCmdInfo.command
                        root.close()
                    }
                }
            }
        }
    }
}
