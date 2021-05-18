import QtQuick 2.12
import QtQuick.Controls 2.12

import "./panelComponents/"
import "./indexPages/"

ToolPanelBase {
    id: textPanle
    
    title: qsTr("文字")
    iconSource: "qrc:/icons/dark/rstoolbar/text.png"
    
    contentItem: Rectangle {
        color: application.darkerSpaceColor
    }
}
