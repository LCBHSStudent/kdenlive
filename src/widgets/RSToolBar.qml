import QtQuick 2.12

Item {
    id: rsBarRoot
    
    Rectangle {
        anchors {
            top: parent.top
            right: parent.right
            rightMargin: -radius
        }

        width: 106 + radius
        height: parent.height
        radius: 6
        color: "#FF3E3D4C"
    }
}
