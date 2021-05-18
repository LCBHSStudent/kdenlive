import QtQuick 2.12
import QtQuick.Controls 2.12
import DFW.Components 1.0

Item {
    id: fieldRoot

    property bool isInteger: true
    TextField {
        horizontalAlignment: TextInput.AlignHCenter

        implicitWidth: 70
        implicitHeight: 20

        selectByMouse: true
        validator: isInteger? intValidator: doubleValidator

        leftPadding: 0
        topPadding: 0
        bottomPadding: 0
        rightPadding: 0

        background: Rectangle {
            anchors.fill: parent
            color: "#FF20202A"
        }

        color: application.buttonFontColor
        font {
            family: application.fontFamily
            pixelSize: 14
        }

//        CursorArea {
//            anchors.fill: parent
//            cursor: CursorArea.IBeamCursor
//        }
    }

    property IntValidator intValidator: IntValidator {}
    property DoubleValidator doubleValidator: DoubleValidator {}
}
