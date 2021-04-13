import QtQuick 2.12
import QtGraphicalEffects 1.12

Item {
    id: rsBarRoot
    
    property int tabBarWidth: 0
    property int tabItemHeight: 0
    property int currentTab: noCurTab
    
    readonly property int noCurTab: -1 
    
    Rectangle {
        id: panel
        radius: 20
        border.width: 1
        border.color: "#FF7781F4"
        width: parent.width + radius
        height: parent.height
        color: "#FF3E3D4C"
        
        PropertyAnimation {
            id: drawOpenAnimation
            target: panel
            property: "x"
            from: rsBarRoot.width - tabBarWidth
            to: 0
            duration: 350
            easing.type: Easing.OutQuart
        }
        PropertyAnimation {
            id: drawCloseAnimation
            target: panel
            property: "x"
            to: rsBarRoot.width - tabBarWidth
            from: 0
            duration: 350
            easing.type: Easing.OutQuart
        }
    }
    
    onCurrentTabChanged: {
        if (currentTab !== noCurTab) {
            drawOpenAnimation.start()
        } else {
            drawCloseAnimation.start()
        }
    }
    
    Rectangle {
        anchors {
            top: parent.top
            right: parent.right
            rightMargin: -radius
        }

        width: tabBarWidth + radius
        height: parent.height
        radius: 6
        color: "#FF3E3D4C"
        
        layer.enabled: true
        layer.effect: DropShadow {
            color: "#AA000000"
            radius: 4
        }
        
        Column {
            anchors {
                top: parent.top
                left: parent.left
                leftMargin: 2
            }
            
            Repeater {
                id: tabRepeater
                model: [
                    [i18n("视频"), "video"],
                    [i18n("音频"), "audio"],
                    [i18n("转场"), "transition"],
                    [i18n("滤镜"), "filter"],
                    [i18n("音乐"), "music"],
                    [i18n("文字"), "text"],
                    [i18n("分屏"), "split_screen"],
                    [i18n("特效"), "effect"]
                ]
                
                delegate: Rectangle {
                    height: tabItemHeight
                    width: tabBarWidth - 2 + radius
                    radius: 6
                    color: index === currentTab?  "#FF7781F4": "transparent"
                    
                    Image {
                        id: tabIcon
                        source: "qrc:/classic/components/rstoolbar/" + modelData[1] + ".png"
                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                            leftMargin: parent.width * 0.154
                        }
                    }
                    
                    Text {
                        text: modelData[0]
                        font {
                            pixelSize: 14
                            family: "Microsoft Yahei"
                        }
                        color: "#E6FFFFFF"
                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                            leftMargin: parent.width * 0.442
                        }
                    }
                    
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (index === currentTab) {
                                currentTab = noCurTab
                            } else {
                                currentTab = index
                            }
                        }
                    }
                }
            }
        }
    }
}
