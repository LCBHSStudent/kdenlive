import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12

Item {
    id: projSettingRoot
    property int __dropshadowMargin
    property int subtitleMarginHor          // 字幕安全区域-水平
    property int subtitleMarginVer          // 字幕安全区域-垂直
    property int actionMarginHor            // 动作安全区域-水平
    property int actionMarginVer            // 动作安全区域-垂直
    
    signal move(var x, var y)
    
    Rectangle {
        anchors.fill: parent
        anchors.bottomMargin: __dropshadowMargin
        radius: 20
        color: "#3E3D4C"
        
        layer.enabled: true
        layer.effect: DropShadow {
            verticalOffset: 6
            radius: 6
            color: "#29000000"
        }
    }
}
