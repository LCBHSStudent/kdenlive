import QtQuick 2.12
import QtQuick.Controls 2.12
import DFW.Components 1.0

StyledButton {
    width: 100
    height: 30
    radius: height / 2
    hoverEnabled: true
    normalColor:    uiconfig.backgroundColor
    pressedColor:   uiconfig.foregroundColor
    border.color:   uiconfig.foregroundColor
    border.width: 1
}
