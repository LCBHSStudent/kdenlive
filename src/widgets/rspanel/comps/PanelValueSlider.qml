import QtQuick 2.12
import QtQuick.Controls 2.12

Slider {
    id: sliderRoot
    implicitWidth: vertical? handle.width: 110
    implicitHeight: vertical? 110: handle.width
    
    opacity: enabled? 1: 0.6
    
    background: Rectangle {
        x: sliderRoot.leftPadding
        y: sliderRoot.topPadding + sliderRoot.availableHeight / 2 - height / 2

        width: vertical? 4: parent.width
        height: vertical? parent.height: 4
        color: "#B8B9C8"
        radius: 2

        anchors.centerIn: parent
    }

    handle: Rectangle {
        x: vertical? 
            sliderRoot.leftPadding + sliderRoot.availableWidth / 2 - width / 2:
            sliderRoot.leftPadding + sliderRoot.visualPosition * (sliderRoot.availableWidth - width)
        y: vertical?
            sliderRoot.topPadding + sliderRoot.visualPosition * (sliderRoot.availableHeight - height):
            sliderRoot.topPadding + sliderRoot.availableHeight / 2 - height / 2

        width: 10
        height: width
        radius: width / 2
        color: "#EAEAEB"
    }
}
