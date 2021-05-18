import QtQuick 2.12
import QtQuick.Controls 2.12

import "./panelComponents/"
import "./indexPages/"

ToolPanelBase {
    id: filterPanle
    
    title: qsTr("滤镜")
    iconSource: "qrc:/icons/dark/rstoolbar/filter.png"
    
    contentItem: Rectangle {
        color: application.darkerSpaceColor
    }
}
