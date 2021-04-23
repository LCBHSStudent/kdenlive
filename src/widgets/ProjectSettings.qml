import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import DFW.Components 1.0

Item {
    id: projSettingRoot
    property int __dropshadowMargin
    property int subtitleMarginHor          // 字幕安全区域-水平
    property int subtitleMarginVer          // 字幕安全区域-垂直
    property int actionMarginHor            // 动作安全区域-水平
    property int actionMarginVer            // 动作安全区域-垂直
    property int profileW                   // 项目宽度
    property int profileH                   // 项目高度
    property real profileFps                // 时基 (帧率)
    
    onProfileFpsChanged: {
        for (var i = 0; i < timebaseCombo.fpsList.length; i++) {
            if (profileFps === timebaseCombo.fpsList[i]) {
                timebaseCombo.currentIndex = i
                break
            }
        }
    }
    
    onSubtitleMarginHorChanged: ssmHor.text = subtitleMarginHor
    onSubtitleMarginVerChanged: ssmVer.text = subtitleMarginVer
    onActionMarginHorChanged: asmHor.text = actionMarginHor
    onActionMarginVerChanged: asmVer.text = actionMarginVer
    
    
    signal move(var x, var y)
    signal close()
    
    UIConfig { id: uiconfig }
    
    Rectangle {
        anchors.fill: parent
        anchors.bottomMargin: __dropshadowMargin
        radius: 20
        color: uiconfig.lighterSpaceColor
        
        layer.enabled: true
        layer.effect: DropShadow {
            verticalOffset: 6
            radius: 6
            color: "#29000000"
        }
        
        TabHeader {
            id: tabBar
            
            tabModel: [
                i18n("常    规"),
                i18n("视    频"),
                i18n("音    频")   
            ]
            
            width: contentBG.width
            anchors {
                bottom: contentBG.top
                horizontalCenter: parent.horizontalCenter
            }
            
            onCurrentIndexChanged: {
                console.debug(tabBar.currentIndex)
            }
        }
        
        Rectangle {
            id: contentBG
            width: 460
            height: 436
            anchors {
                bottom: parent.bottom
                bottomMargin: 80
                horizontalCenter: parent.horizontalCenter
            }
            radius: 10
            color: "#2D2C39"
            
            Rectangle {
                width: parent.width
                height: parent.radius
                color: parent.color
            }
            
            StackLayout {
                id: contentStack
                currentIndex: tabBar.currentIndex
                anchors.fill: parent
                
                Page {
                    id: regularPage
                    
                    ThemeText {
                        id: timebaseText
                        color: uiconfig.lighterFontColor
                        anchors {
                            right: parent.right
                            rightMargin: 311
                            top: parent.top
                            topMargin: 56
                        }
                        text: i18n("时    基  ")
                    }
                    StyledComboBoxRect {
                        id: timebaseCombo
                        
                        property var fpsList: [
                            15.0, 23.976, 24.0, 25.0, 29.97, 30.0, 49.9994, 50.0, 59.9401, 60
                        ]
                        
                        model: [
                            i18n("15.00         帧/秒"),
                            i18n("23.976       帧/秒"),
                            i18n("24.00         帧/秒"),
                            i18n("25.00         帧/秒"),
                            i18n("29.97         帧/秒"),
                            i18n("30.00         帧/秒"),
                            i18n("49.9994     帧/秒"),
                            i18n("50.00         帧/秒"),
                            i18n("59.9401     帧/秒"),
                            i18n("60.00         帧/秒")
                        ]
                        anchors {
                            left: timebaseText.right
                            leftMargin: 12
                            verticalCenter: timebaseText.verticalCenter
                        }
                    }
                    
                    ThemeText {
                        id: subtitleSafeMarginText
                        color: uiconfig.lighterFontColor
                        anchors {
                            right: parent.right
                            rightMargin: 311
                            top: parent.top
                            topMargin: 116
                        }
                        text: i18n("字幕安全区域  ")
                    }
                    
                    StyledTextField {
                        id: ssmHor
                        
                        anchors {
                            left: timebaseCombo.left
                            verticalCenter: subtitleSafeMarginText.verticalCenter
                        }

                        width: 54
                        height: 30
                        validator: IntValidator {
                            top: profileH
                            bottom: 0
                        }
                    }
                    
                    StyledTextField {
                        id: ssmVer
                        
                        anchors {
                            left: ssmHor.right
                            verticalCenter: subtitleSafeMarginText.verticalCenter
                        }
                        width: 54
                        height: 30
                        validator: IntValidator {
                            top: profileW
                            bottom: 0
                        }
                    }
                    
                    ThemeText {
                        id: actionSafeMarginText
                        color: uiconfig.lighterFontColor
                        anchors {
                            right: parent.right
                            rightMargin: 311
                            top: parent.top
                            topMargin: 157
                        }
                        text: i18n("动作安全区域  ")
                    }
                    
                    StyledTextField {
                        id: asmHor
                        
                        anchors {
                            left: timebaseCombo.left
                            verticalCenter: actionSafeMarginText.verticalCenter
                        }
                        width: 54
                        height: 30
                        validator: IntValidator {
                            top: profileH
                            bottom: 0
                        }
                    }
                    
                    StyledTextField {
                        id: asmVer
                        width: 54
                        height: 30
                        
                        anchors {
                            left: asmHor.right
                            verticalCenter: actionSafeMarginText.verticalCenter
                        }
                        validator: IntValidator {
                            top: profileW
                            bottom: 0
                        }
                    }
                    
                }
                
                
                Page {
                    id: videoPage
                }
                
                
                Page {
                    id: audioPage
                }
                
                
            }

            
        }
        
        Row {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 30
            }
            spacing: 80
            
            PopupButton {
                text: i18n("确    定")
                anchors.bottom: parent.bottom
                
                onClicked: {
                    
                }
            }
            PopupButton {
                text: i18n("取    消")
                anchors.bottom: parent.bottom
                
                onClicked: {
                    close()
                }
            }
        }
    }
}
