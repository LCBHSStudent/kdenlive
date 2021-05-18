import QtQuick 2.12
import QtQuick.Controls 2.12

import "./panelComponents/"
import "./indexPages/"

ToolPanelBase {
    id: splitPanle
    
    title: qsTr("分屏")
    iconSource: "qrc:/icons/dark/rstoolbar/split_screen.png"
    
    contentItem: Rectangle {
        color: application.darkerSpaceColor
    }
}
