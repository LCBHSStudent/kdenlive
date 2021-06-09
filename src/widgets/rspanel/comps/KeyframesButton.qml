import DFW.Components 1.0

IconToolButton {
    iconSize: 14
    
    hoverEnabled: false
    
    iconOpacity: checked? 1: 0.8
    
    iconSource: "qrc:/classic/controllers/btn_add_keyframe.png"

    anchors.verticalCenter: parent.verticalCenter

    checkable: true
}
