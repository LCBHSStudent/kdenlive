import QtQuick 2.12
import QtQuick.Controls 2.12

import "./panelComponents/"
import "./indexPages/"

ToolPanelBase {
    id: musicPanle
    
    title: qsTr("音乐")
    iconSource: "qrc:/icons/dark/rstoolbar/music.png"
    
    contentItem: Rectangle {
        color: application.darkerSpaceColor
    }
}
