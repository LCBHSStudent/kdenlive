import QtQuick 2.12
import QtQuick.Controls 2.12
import DFW.Components 1.0

Row {
    id: tabBarRoot
    
    property var tabModel: []
    
    height: tabModel.length? 40: 0
    spacing: 0
    
    property int currentIndex: 0

    Repeater {
        model: tabModel
        delegate: TabButton {
            id: tabBtn
        
            property bool selected: index === tabBarRoot.currentIndex
            
            text: modelData
            width: tabModel.length? tabBarRoot.width / tabModel.length: 0
            height: tabBarRoot.height
            
            font {
                pixelSize: 14
                bold: tabBtn.selected
                family: uiconfig.fontFamily
            }
        
            contentItem: ThemeText {
                font: tabBtn.font
                text: tabBtn.text
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
        
                color: uiconfig.normalFontColor
            }
            
            hoverEnabled: true
            
            background: Rectangle {
                anchors.fill: parent
                color: tabBtn.selected? uiconfig.darkerSpaceColor: tabBtn.hovered? Qt.darker(uiconfig.lighterSpaceColor, 1.1): uiconfig.lighterSpaceColor
                Rectangle {
                    width: parent.width
                    height: parent.radius
                    color: parent.color
                    anchors.bottom: parent.bottom
                }
            }
            
            onClicked: {
                tabBarRoot.currentIndex = index
            }
        }
    }
}     
