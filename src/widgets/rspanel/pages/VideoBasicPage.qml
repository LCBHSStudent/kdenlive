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

    layer.enabled: true
    layer.samples: 10

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
                serviceName: "size_position"
                serviceEnum: Constants.SizePosition
                section: qsTr("位置与尺寸")
                content: Column {
                    SectionContentItem {
                        roleText: qsTr("位置")
                        centralPart: Item {
                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                PanelValueField {}
                            }
                        }

                        onResetValue: {

                        }
                        onKeyframeBtnClicked: {
                            
                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("大小")
                        centralPart: Item {

                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("缩放")
                        centralPart: Item {

                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("旋转")
                        centralPart: Item {

                        }
                    }

                    SectionContentItem {
                        roleText: qsTr("翻转")

                        enableKeyframe: false
                        enableResetValue: false
                        centralPart: Item {
                            IconToolButton {
                                width: 28; height: 28
                                anchors.verticalCenter: parent.verticalCenter
                                iconSource: "qrc:/icons/dark/toolpanel/horizontal_rotate.png"

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
                                width: 28; height: 28
                                x: 48
                                anchors.verticalCenter: parent.verticalCenter
                                iconSource: "qrc:/icons/dark/toolpanel/vertical_rotate.png"
                    
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
