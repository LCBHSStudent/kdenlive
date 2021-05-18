import QtQuick 2.12
import QtGraphicalEffects 1.12

import "./rspanel/"

import DFW.Components 1.0

Item {
    id: rsBarRoot
    
    property int tabBarWidth: 0
    property int tabItemHeight: 0
    property int currentTab: noCurTab
    
    readonly property int noCurTab: -1
    
    UIConfig { id: uiconfig }
    
    Loader {
        id: panel
        width: parent.width
        height: parent.height
        x: rsBarRoot.width - tabBarWidth
        
        sourceComponent: VideoPanel {}
        
        state: "close"
        
        states: [
            State {
                name: "close"
                PropertyChanges {
                    target: panel
                    x: rsBarRoot.width - tabBarWidth
                }
            },
            State {
                name: "open"
                PropertyChanges {
                    target: panel
                    x: 0
                }
            }

        ]
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
                                panel.state = "close"
                            } else {
                                currentTab = index
                                panel.state = "open"
                            }
                        }
                    }
                }
            }
        }
    }
}
