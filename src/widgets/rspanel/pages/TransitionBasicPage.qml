import QtQuick 2.12
import QtQuick.Controls 2.12

import DFW.Components 1.0

Page {
    background: Item {}

    property alias ctentHeight: flicker.contentHeight
    property alias ctentY: flicker.contentY
    
    layer.enabled: true
    layer.samples: 10000

    Flickable {
        id: flicker
        clip: true
        anchors.fill: parent
        contentHeight: contentCol.height
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: contentCol
            anchors {
                left: parent.left
                top: parent.top
            }
            
            Item { width: 1; height: 9 }
            
            StyledCheckable {
                id: selectFavorite
                text: qsTr("仅显示我的收藏")
                indicatorBorder: true
                x: 27
                checked: false

                onCheckedChanged: {
                    transModal.filter()
                }
            }
            Item { width: 1; height: 9 }
            
            Grid {
                id: grid
                x: 17
                columns: 3
                columnSpacing: 13
                rowSpacing: 8.5
    
                Repeater {
                    model: transModal
                }
            }
        }
    }
    
    Component.onCompleted: {
        transModal.filter()
    }

    FilterProxyModel {
        id: transModal
        
        filterRule: function(item) {
            if (mixctrl.getFavorite(item.value) && selectFavorite.checked || !selectFavorite.checked) {
                return true
            } else {
                return false
            }
        }
        
        model: ListModel {
            ListElement {
                img: "fade_in_out"
                name: qsTr("渐入渐出")
                value: 0
            }
            ListElement {
                img: "horizontally_erase"
                name: qsTr("横向擦除")
                value: 2
            }
            ListElement {
                img: "vertically_erase"
                name: qsTr("纵向擦除")
                value: 3
            }
            ListElement {
                img: "horizontally_close"
                name: qsTr("水平对开")
                value: 4
            }
            ListElement {
                img: "vertically_close"
                name: qsTr("纵向对开")
                value: 5
            }
            ListElement {
                img: "obliquely_close_1"
                name: qsTr("斜角对开1")
                value: 6
            }
            ListElement {
                img: "obliquely_close_2"
                name: qsTr("斜角对开2")
                value: 7
            }
            ListElement {
                img: "obliquely_erase_1"
                name: qsTr("倾斜擦除1")
                value: 8
            }
            ListElement {
                img: "obliquely_erase_2"
                name: qsTr("倾斜擦除2")
                value: 9
            }
            ListElement {
                img: "horiztontal_waterfall"
                name: qsTr("水平瀑布")
                value: 10
            }
            ListElement {
                img: "vertical_waterfall"
                name: qsTr("垂直瀑布")
                value: 11
            }
            ListElement {
                img: "horizontal_snake_1"
                name: qsTr("水平蛇形1")
                value: 12
            }
            ListElement {
                img: "horizontal_snake_2"
                name: qsTr("水平蛇形2")
                value: 13
            }
            ListElement {
                img: "vertical_snake_1"
                name: qsTr("垂直蛇形1")
                value: 14
            }
            ListElement {
                img: "vertical_snake_2"
                name: qsTr("垂直蛇形2")
                value: 15
            }
            ListElement {
                img: "v_shaped_symmetry"
                name: qsTr("V字对称")
                value: 16
            }
            ListElement {
                img: "circular_diffusion"
                name: qsTr("圆形扩散")
                value: 17
            }
            ListElement {
                img: "double_circle_diffusion"
                name: qsTr("双圆形扩散")
                value: 18
            }
            ListElement {
                img: "rectangular_diffusion_1"
                name: qsTr("矩形扩散1")
                value: 20
            }
            ListElement {
                img: "rectangular_diffusion_2"
                name: qsTr("矩形扩散2")
                value: 21
            }
            ListElement {
                img: "rectangular_diffusion_3"
                name: qsTr("矩形扩散3")
                value: 22
            }
            ListElement {
                img: "clockwise_erase"
                name: qsTr("顺时针擦除")
                value: 23
            }
            ListElement {
                img: "counter_clockwise_erase"
                name: qsTr("逆时针擦除")
                value: 123
            }
        }
        Rectangle {
            width: 120; height: 130
            radius: 6
            color: "transparent"
            border.width: value === mixctrl.lumaMixType? 1: 0
            border.color: application.foregroundColor

            function isMouseInBtn(x, y) {
                if (x >= favoriteChecker.x && x <= favoriteChecker.x + favoriteChecker.width) {
                    if (y >= favoriteChecker.y && y <= favoriteChecker.y + favoriteChecker.height) {
                        return true
                    }
                }
                return false
            }

            MouseArea {
                id: mArea
                anchors.fill: parent
                onPressed: {
                    if (isMouseInBtn(mouseX, mouseY)) {
                        favoriteChecker.opacity = 0.6
                    }
                }
                onReleased: {
                    favoriteChecker.opacity = 1
                }
                onClicked: {
                    if (isMouseInBtn(mouseX, mouseY)) {
                        favoriteChecker.checked = !favoriteChecker.checked
                        mixctrl.setFavorite(value, favoriteChecker.checked)
                        if (favoriteChecker.checked) {
                            transModal.filter()   
                        }
                    } else {
                        mixctrl.setLumaMixType(value)
                    }
                }
            }

            Image {
                id: transImg
                width: 100; height: 80

                source: "qrc:/icons/dark/toolpanel/transition/" + img + ".png"
                mipmap: true
                anchors {
                    top: parent.top
                    topMargin: 10
                    horizontalCenter: parent.horizontalCenter
                }
            }
            ThemeText {
                id: transName
                text: name
                anchors {
                    top: transImg.bottom
                    topMargin: 7
                    left: transImg.left
                    right: favoriteChecker.left
                    rightMargin: 4
                }
                elide: Text.ElideRight
            }
            Image {
                id: favoriteChecker
                property bool checked: mixctrl.getFavorite(value)

                width: 22; height: 22
                fillMode: Image.PreserveAspectFit

                anchors {
                    bottom: transName.bottom
                    right: transImg.right
                }
                smooth: true
                
                source: "qrc:/icons/dark/toolpanel/favorite_" + (favoriteChecker.checked? "checked": "unchecked") + ".png"
            }
        }
    }
}
