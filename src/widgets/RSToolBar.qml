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
    
    Component {
        id: videoPanel
        VideoPanel {}
    }
    
    Loader {
        id: panel
        width: parent.width
        height: parent.height
        x: rsBarRoot.width - tabBarWidth
        
        sourceComponent: {
            switch(currentTab) {
            case 0:
                return videoPanel 
            default: 
                return null
            }
        }
        
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
        color: "#FF3E3D4C"
        
        layer.enabled: currentTab === noCurTab
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
                
                delegate: Item {
                    height: tabItemHeight
                    width: tabBarWidth - 2
                    
                    Image {
                        id: tabIcon
                        source: "qrc:/classic/components/rstoolbar/" + modelData[1] + ".png"
                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                            leftMargin: parent.width * 0.154
                        }
                        
                        layer.enabled: index === currentTab
                        layer.effect: ColorOverlay {
                            color: uiconfig.foregroundColor
                        }
                    }
                    
                    Rectangle {
                        visible: index === currentTab
                        anchors {
                            right: parent.right
                            rightMargin: 6
                            verticalCenter: parent.verticalCenter
                        }
                        width: 4
                        height: 30
                        radius: width
                        color: uiconfig.foregroundColor
                    }
                    
                    Text {
                        text: modelData[0]
                        font {
                            bold: true
                            pixelSize: 14
                            family: "Microsoft Yahei"
                        }
                        color: index === currentTab? uiconfig.foregroundColor: uiconfig.lighterFontColor
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
