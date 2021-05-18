import QtQuick 2.12
import QtQuick.Controls 2.12

import "./panelComponents/"
import "./indexPages/"

ToolPanelBase {
    id: audioPanle
    
    title: qsTr("音频")
    iconSource: "qrc:/icons/dark/rstoolbar/audio.png"
    
    contentItem: Rectangle {
        color: application.darkerSpaceColor
    }
}
