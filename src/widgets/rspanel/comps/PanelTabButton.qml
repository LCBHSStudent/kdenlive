import QtQuick 2.12
import QtQuick.Controls 2.12

import DFW.Components 1.0

TabButton {
    id: control

    property bool selected: false

    font {
        pixelSize: 14
        bold: selected
        family: uiconfig.fontFamily
    }

    contentItem: ThemeText {
        font: control.font
        text: control.text
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
    }

    background: Rectangle {
        anchors.fill: parent
        color: selected? uiconfig.darkerSpaceColor: control.down? Qt.darker(uiconfig.lighterSpaceColor, 1.1): uiconfig.lighterSpaceColor
        radius: 10
        Rectangle {
            width: parent.width
            height: parent.radius
            color: parent.color
            anchors.bottom: parent.bottom
        }
    }
}
