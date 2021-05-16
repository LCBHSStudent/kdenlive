import QtQuick 2.12
import QtQuick.Controls 2.12
import DFW.Components 1.0

StyledButton {
    width: 86
    height: 30
    radius: 0
    hoverEnabled: true
    normalColor:    uiconfig.backgroundColor
    pressedColor:   Qt.darker(uiconfig.backgroundColor, 1.2)
    border.color:   uiconfig.controlBorderColor
    border.width: 1
}
