import QtQuick 2.12
import QtQuick.Controls 2.12

import "./panelComponents/"
import "./indexPages/"

ToolPanelBase {
    id: effectPanle
    
    title: qsTr("特效")
    iconSource: "qrc:/icons/dark/rstoolbar/effect.png"
    
    contentItem: Rectangle {
        color: application.darkerSpaceColor
    }
}
