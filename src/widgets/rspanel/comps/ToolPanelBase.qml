import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import DFW.Components 1.0

/**
 @comment: 左侧有着20px的margin, 注意调整BG相关位置
 */

Item {
    id: __toolPanelBase_qml
    
    anchors.fill: parent
    anchors.rightMargin: tabBarWidth
    
    property StackLayout    contentItem: null
    property Item           currentItem: contentItem.children[contentItem.currentIndex]

    property var tabModel: []
    property var positionModel: []
    
    // control content flickable
    signal moveToPos(var position)
    
    Rectangle {
        id: contentItemBase
        color: uiconfig.lighterSpaceColor
        radius: 10
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: contentScrollBar.left
            left: parent.left
            topMargin: 63
            bottomMargin: 20
            rightMargin: 0
            leftMargin: 0
        }

        Rectangle {
            width: parent.width
            height: parent.radius
            color: parent.color
            anchors.top: parent.top
        }

        Row {
            id: tabBar
            width: parent.width
            height: tabModel.length? 45: 0
            spacing: 0
            
            property int currentIndex: 0

            anchors {
                bottom: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            Repeater {
                model: tabModel
                delegate: PanelTabButton {
                    text: modelData
                    width: tabModel.length? tabBar.width / tabModel.length: 0
                    height: tabBar.height

                    selected: index === tabBar.currentIndex

                    onClicked: {
                        tabBar.currentIndex = index
                    }
                }
            }
        }
    }
    StyledScrollBar {
        id: contentScrollBar
        width: 8
        height: contentItemBase.height
        anchors {
            verticalCenter: contentItemBase.verticalCenter
            right: parent.right
        }

        size: height / currentItem.ctentHeight
        position: currentItem.ctentY / currentItem.ctentHeight

        onPositionChanged: {
            currentItem.ctentY = position * currentItem.ctentHeight
        }

        contentItem.visible: currentItem.ctentHeight > height
        indicatorPadding: 2
    }

    onContentItemChanged: {
        contentItem.parent          = contentItemBase
        contentItem.anchors.top     = Qt.binding(function() { return contentItemBase.top })
        contentItem.anchors.bottom  = Qt.binding(function() { return contentItemBase.bottom })
        contentItem.anchors.left    = Qt.binding(function() { return contentItemBase.left })
        contentItem.anchors.right   = Qt.binding(function() { return contentItemBase.right })
        contentItem.currentIndex    = Qt.binding(function() { return tabBar.currentIndex })
    }
}
