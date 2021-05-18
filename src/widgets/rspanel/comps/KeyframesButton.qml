import DFW.Components 1.0

IconToolButton {
    width: 28
    height: 28

    hoverEnabled: false
    bgColor: checked? application.buttonNormalColor: "#00000000"
    iconSource: "qrc:/icons/dark/toolpanel/keyframe.png"

    anchors.verticalCenter: parent.verticalCenter

    checkable: true
}
