import QtQuick 2.12
import QtQuick.Controls 2.12
import DFW.Components 1.0

Item {
    id: fieldRoot
    width: prefix.width + suffix.width + field.width + 2 * contentRow.spacing
    height: field.height
    property bool isInteger: true
    property alias prefix: prefix.text
    property alias suffix: suffix.text
    property alias margin: contentRow.spacing
    property alias value: field.text
    
    property string textSuffix: ""
    
    FontMetrics {
        id: fm
        font: field.font
    }
    
    anchors.verticalCenter: parent.verticalCenter
    
    Row {
        id: contentRow
        anchors.verticalCenter: parent.verticalCenter
        ThemeText {
            id: prefix        
        }
        TextField {
            id: field
            horizontalAlignment: TextInput.AlignHCenter
            verticalAlignment: TextInput.AlignVCenter
            
            implicitWidth: 60
            implicitHeight: 20
    
            selectByMouse: true
            validator: isInteger? intValidator: doubleValidator
            
            topPadding: 2
            bottomPadding: 2
            leftPadding: 0
            rightPadding: 0
    
            background: Rectangle {
                anchors.fill: parent
                color: "#FF20202A"
            }
    
            color: "transparent"
            font {
                family: uiconfig.fontFamily
                pixelSize: 12
            }
            
            
            cursorDelegate: cursor
            Component {
                id:cursor
                Rectangle{
                    id: cursorRect
                    width: 1
                    color: uiconfig.normalFontColor
                    
                    Timer {
                        running: field.focus
                        repeat: true
                        interval: 500
                        onTriggered: {
                            cursorRect.visible = !cursorRect.visible
                        }

                        onRunningChanged: {
                            if (running === false) {
                                cursorRect.visible = false
                            }
                        }
                    }
                }
            }
            
            CursorArea {
                anchors.fill: parent
                cursor: CursorArea.IBeamCursor
            }
            
            Text {
                id: suffixText
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: parent.height
                verticalAlignment: parent.verticalAlignment
                anchors.horizontalCenterOffset: fm.advanceWidth(textSuffix)
                horizontalAlignment: parent.horizontalAlignment
                font: parent.font
                color: uiconfig.normalFontColor
                text: parent.displayText + textSuffix
            }
        }
        
        ThemeText {
            id: suffix        
        }
    }
    
    Connections {
        target: uiconfig
        function onResetFocus() {
            field.focus = false
        }
    }    

    property IntValidator intValidator: IntValidator {}
    property DoubleValidator doubleValidator: DoubleValidator {}
}
