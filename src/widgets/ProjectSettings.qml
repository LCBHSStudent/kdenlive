import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import DFW.Components 1.0

import "qrc:/qml/widgets/"

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
    
    property int aspectNum                  // 横纵比: 水平
    property int aspectDen                  // 横纵比: 垂直
    property int scanMethod                 // 扫描方式
    
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
    onProfileWChanged: resolutionW.text = profileW
    onProfileHChanged: resolutionH.text = profileH
    
    
    signal move(var x, var y)
    signal cancel()
    signal confirm()
    
    UIConfig { id: uiconfig }
    
    Rectangle {
        anchors.fill: parent
        anchors.bottomMargin: __dropshadowMargin
        color: uiconfig.lighterSpaceColor
        
        MouseArea {
            id: movableHeader
            anchors {
                top: parent.top
                bottom: tabBar.top
                left: parent.left
                right: parent.right
            }
            
            property int pressX
            property int pressY
            
            onPressed: {
                cursorShape = Qt.SizeAllCursor
                pressX = mouseX
                pressY = mouseY
            }
            
            onPositionChanged: {
                move(mouseX - pressX, mouseY - pressY)
            }

            onReleased: {
                cursorShape = Qt.ArrowCursor
            }
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
                            [i18n("15.00         帧/秒"), ""],
                            [i18n("23.976       帧/秒"), ""],
                            [i18n("24.00         帧/秒"), ""],
                            [i18n("25.00         帧/秒"), ""],
                            [i18n("29.97         帧/秒"), ""],
                            [i18n("30.00         帧/秒"), ""],
                            [i18n("49.9994     帧/秒"), ""],
                            [i18n("50.00         帧/秒"), ""],
                            [i18n("59.9401     帧/秒"), ""],
                            [i18n("60.00         帧/秒"), ""]
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
                        suffix: i18n("px 水平")
                    }
                    
                    StyledTextField {
                        id: ssmVer
                        
                        anchors {
                            left: ssmHor.right
                            leftMargin: 80
                            verticalCenter: subtitleSafeMarginText.verticalCenter
                        }
                        width: 54
                        height: 30
                        validator: IntValidator {
                            top: profileW
                            bottom: 0
                        }
                        suffix: i18n("px 垂直")
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
                        suffix: i18n("px 水平")
                    }
                    
                    StyledTextField {
                        id: asmVer
                        width: 54
                        height: 30
                        
                        anchors {
                            left: asmHor.right
                            leftMargin: 80
                            verticalCenter: actionSafeMarginText.verticalCenter
                        }
                        validator: IntValidator {
                            top: profileW
                            bottom: 0
                        }
                        suffix: i18n("px 垂直")
                    }
                    
                }
                
                /************************************PAGE VIDEO******************************/
                Page {
                    id: videoPage
                    
                    ThemeText {
                        id: aspectText
                        color: uiconfig.lighterFontColor
                        text: i18n("宽高比例")
                        anchors {
                            right: parent.right
                            rightMargin: 318
                            top: parent.top
                            topMargin: 56
                        }
                    }
                    
                    StyledComboBoxRect {
                        id: aspectCombo
                        width: 210
                        
                        anchors {
                            left: aspectText.right
                            leftMargin: 20
                            verticalCenter: aspectText.verticalCenter
                        }
                        
                        model: [
                            [i18n("9 : 16 (竖屏)  抖音"), "qrc:/classic/components/icon_fire.png"],
                            [i18n("16 : 9 (宽屏)"), "qrc:/classic/components/icon_bilibili.png"],
                            [i18n("1 : 1  Instagram"), ""],
                            [i18n("4 : 3 (标准)"), ""],
                            [i18n("3 : 4 (电商)"), ""],
                            [i18n("21: 9 (影院)"), ""],
                            [i18n("自定义"), ""],
                        ]
                    }
                    
                    ThemeText {
                        id: resolutionText
                        color: uiconfig.lighterFontColor
                        text: i18n("分辨率")
                        anchors {
                            right: parent.right
                            rightMargin: 318
                            top: parent.top
                            topMargin: 128
                        }
                    }
                    
                    StyledTextField {
                        id: resolutionW
                        width: 73.5
                        anchors {
                            left: aspectCombo.left
                            verticalCenter: resolutionText.verticalCenter
                        }
                        suffix: "X"
                        suffixMargin: 10
                        validator: IntValidator {
                            bottom: 0
                            top: 9999
                        }
                    }
                    
                    StyledTextField {
                        id: resolutionH
                        width: 73.5
                        anchors {
                            left: resolutionW.right
                            leftMargin: 29
                            verticalCenter: resolutionText.verticalCenter
                        }
                        validator: IntValidator {
                            bottom: 0
                            top: 9999
                        }
                    }
                    
                    ThemeText {
                        id: scanText
                        color: uiconfig.lighterFontColor
                        text: i18n("扫描方式")
                        anchors {
                            right: parent.right
                            rightMargin: 318
                            top: parent.top
                            topMargin: 188
                        }
                    }
                    
                    StyledComboBoxRect {
                        id: scanCombo
                        width: 99
                        anchors {
                            left: aspectCombo.left
                            verticalCenter: scanText.verticalCenter
                        }
                        
                        model: [
                            [i18n("逐行"), ""],
                            [i18n("隔行"), ""]
                        ]
                    }
                }
                
                /************************************PAGE AUDIO******************************/
                Page {
                    id: audioPage
                    
                    ThemeText {
                        id: channelText
                        color: uiconfig.lighterFontColor
                        text: i18n("声道")
                        anchors {
                            right: parent.right
                            rightMargin: 318
                            top: parent.top
                            topMargin: 56
                        }
                    }
                    
                    StyledComboBoxRect {
                        id: channelCombo
                        width: 112
                        anchors {
                            left: channelText.right
                            leftMargin: 20
                            verticalCenter: channelText.verticalCenter
                        }
                        
                        model: [
                            i18n("立体声"),
                            i18n("单声道"),
                            i18n("6（5.1）")
                        ]
                    }
                    
                    ThemeText {
                        id: sampleText
                        color: uiconfig.lighterFontColor
                        text: i18n("采样率")
                        anchors {
                            right: parent.right
                            rightMargin: 318
                            top: parent.top
                            topMargin: 116
                        }
                    }
                    
                    StyledComboBoxRect {
                        id: sampleCombo
                        width: 113
                        anchors {
                            left: channelCombo.left
                            verticalCenter: sampleText.verticalCenter
                        }
                        
                        model: [
                            48000
                        ]
                    }
                    
                    ThemeText {
                        color: uiconfig.lighterFontColor
                        anchors {
                            left: sampleCombo.right
                            leftMargin: 10
                            verticalCenter: sampleCombo.verticalCenter
                        }
                        text: "Hz"
                    }
                    
                    ThemeText {
                        id: codecText
                        color: uiconfig.lighterFontColor
                        text: i18n("编码器")
                        anchors {
                            right: parent.right
                            rightMargin: 318
                            top: parent.top
                            topMargin: 176
                        }
                    }
                    
                    StyledComboBoxRect {
                        id: codecCombo
                        width: 111
                        anchors {
                            left: channelCombo.left
                            verticalCenter: codecText.verticalCenter
                        }
                        
                        model: [
                            "acc"
                        ]
                    }
                    
                    
                    ThemeText {
                        id: codecRateCtrlText
                        color: uiconfig.lighterFontColor
                        text: i18n("编码率控制")
                        anchors {
                            right: parent.right
                            rightMargin: 318
                            top: parent.top
                            topMargin: 236
                        }
                    }
                    
                    StyledComboBoxRect {
                        id: codecRateCtrlCombo
                        width: 174
                        anchors {
                            left: channelCombo.left
                            verticalCenter: codecRateCtrlText.verticalCenter
                        }
                        
                        model: [
                            i18n("平均码率（ABR）"),
                        ]
                    }
                    
                    ThemeText {
                        id: bitRateText
                        color: uiconfig.lighterFontColor
                        text: i18n("码率")
                        anchors {
                            right: parent.right
                            rightMargin: 318
                            top: parent.top
                            topMargin: 296
                        }
                    }
                    
                    StyledComboBoxRect {
                        id: bitRateCombo
                        width: 103
                        anchors {
                            left: channelCombo.left
                            verticalCenter: bitRateText.verticalCenter
                        }
                        
                        model: [
                            "384k"
                        ]
                    }
                    
                    ThemeText {
                        color: uiconfig.lighterFontColor
                        anchors {
                            left: bitRateCombo.right
                            leftMargin: 17
                            verticalCenter: bitRateCombo.verticalCenter
                        }
                        text: i18n("比特/秒")
                    }
                    
                    StyledCheckable {
                        indicatorBorder: false
                        text: i18n("禁用音频")
                        leftPadding: 0
                        anchors {
                            left: channelCombo.left
                            top: parent.top
                            topMargin: 352
                        }
                    }
                }
            }
        }
        
        Row {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 30
            }
            spacing: 105
            
            PopupButton {
                text: i18n("确    定")
                anchors.bottom: parent.bottom
                
                onClicked: {
                    confirm()
                }
            }
            PopupButton {
                text: i18n("取    消")
                anchors.bottom: parent.bottom
                
                onClicked: {
                    cancel()
                }
            }
        }
    }
}
