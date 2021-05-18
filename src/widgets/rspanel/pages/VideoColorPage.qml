import QtQuick 2.12
import QtQuick.Controls 2.12

import DFW.Components 1.0

import Shotcut.Controls 1.0 as SC

import "../comps"
import "VideoPagesHelper.js" as VIP

Page {
    background: Item {}

    property alias ctentHeight: flicker.contentHeight
    property alias ctentY: flicker.contentY

    Flickable {
        id: flicker
        clip: true
        anchors.fill: parent
        contentHeight: contentCol.height
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: contentCol
        }
    }
}
