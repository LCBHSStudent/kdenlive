import QtQuick 2.12
import QtQuick.Controls 2.12

import DFW.Components 1.0

import Shotcut.Controls 1.0 as SC

import "../comps/"
import "VideoPagesHelper.js" as VIP

Page {
    background: Item {}

    property alias ctentHeight: flicker.contentHeight
    property alias ctentY: flicker.contentY
    
    Flickable {
        id: flicker
        clip: true
        anchors.fill: parent
        contentHeight: contentCol.height
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: contentCol
            width: parent.width
            Item {width: 1; height: 30}

            ServiceControlBlock {
                id: sizePositionBlock
                section: qsTr("位置与尺寸")
                
                onServiceSelectedChanged: {
                    if (serviceSelected) {
                        console.debug(assetCtrl.selectSizePositionAdjust())
                    }
                }
                
                content: Column {
                    SectionContentItem {
                        roleText: qsTr("位置")
                        centralPart: Item {
                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                PanelValueField {
                                    prefix: "X"
                                    margin: 10
                                }
                                PanelValueField {
                                    prefix: "，Y"
                                    margin: 10
                                }
                            }
                        }

                        onResetValue: {

                        }
                        onKeyframeToggled: {
                            
                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("大小")
                        centralPart: Item {
                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                PanelValueField {
                                    prefix: "W"
                                    margin: 7
                                }
                                PanelValueField {
                                    prefix: "，H"
                                    margin: 10
                                }
                            }
                        }

                        onResetValue: {

                        }
                        onKeyframeToggled: {
                            
                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("缩放")
                        centralPart: Item {
                            Row {
                                spacing: 15
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                PanelValueSlider {
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                PanelValueField {
                                    fieldWidth: 50
                                    textSuffix: "%"
                                }
                            }
                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("旋转")
                        centralPart: Item {
                            Row {
                                spacing: 15
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                PanelValueSlider {
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                PanelValueField {
                                    fieldWidth: 50
                                    textSuffix: "%"
                                }
                            }
                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("翻转")

                        enableKeyframe: false
                        enableResetValue: false
                        centralPart: Item {
                            IconToolButton {
                                iconSize: 28
                                anchors.verticalCenter: parent.verticalCenter
                                iconSource: "qrc:/classic/controllers/btn_horizontal_rotate.png"

                                onClicked: {
                                    if (filter.getInt(horziontalRotationProperty) === 0) {
                                        filter.set(horziontalRotationProperty, 180)
                                    } else {
                                        filter.set(horziontalRotationProperty, 0)
                                    }
                                }

                                // tooltip: qsTr("水平翻转")
                            }
                            IconToolButton {
                                iconSize: 28
                                x: 48
                                anchors.verticalCenter: parent.verticalCenter
                                iconSource: "qrc:/classic/controllers/btn_vertical_rotate.png"
                    
                                onClicked: {
                                    if (filter.getInt(verticalRotationProperty) === 0) {
                                        filter.set(verticalRotationProperty, 180)
                                    } else {
                                        filter.set(verticalRotationProperty, 0)
                                    }
                                }

                                // tooltip: qsTr("垂直翻转")
                            }
                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("背景")

                        enableKeyframe: false
                        centralPart: Item {
                            SC.ColorPicker {
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                    }
                }
            }

            ServiceControlBlock {
                id: mixBlock
                section: qsTr("混合选项")
                content: Column {
                    SectionContentItem {
                        roleText: qsTr("混合模式")

                        enableKeyframe: false
                        centralPart: Item {

                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("不透明度")
                        centralPart: Item {

                        }
                    }
                }
            }

            ServiceControlBlock {
                id: lenCorrectionBlock
                section: qsTr("镜头矫正")
                content: Column {
                    SectionContentItem {
                        roleText: qsTr("X中心")

                        enableKeyframe: false
                        centralPart: Item {

                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("Y中心")
                        
                        enableKeyframe: false
                        centralPart: Item {

                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("中心修正")
                        
                        enableKeyframe: false
                        centralPart: Item {

                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("边缘修正")
                        
                        enableKeyframe: false
                        centralPart: Item {

                        }
                    }
                }
            }
        }
    }
}
