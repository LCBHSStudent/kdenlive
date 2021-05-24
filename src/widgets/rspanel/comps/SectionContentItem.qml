import QtQuick 2.12
import QtQuick.Controls 2.12
import DFW.Components 1.0

Item {
    width: parent.width
    height: 43

    property bool enableKeyframe: true
    property bool enableResetValue: true
    property alias roleText: __roleText.text

    signal resetValue()
    signal keyframeToggled(var checked)

    function toggleKeyframe(checked) { keyframeBtn.checked = checked }

    ThemeText {
        id: __roleText
        anchors {
            right: parent.right
            rightMargin: parent.width - 60
            verticalCenter: parent.verticalCenter
        }
        horizontalAlignment: Text.AlignRight
    }

    property Item centralPart: null

    onCentralPartChanged: {
        centralPart.parent = this
        centralPart.height = this.height
        centralPart.anchors.left = __roleText.right
        centralPart.anchors.right = this.right
        centralPart.anchors.leftMargin = 17
        centralPart.anchors.rightMargin = 62
    }

    ResetValueButton {
        visible: enableResetValue
        onClicked: {
            resetValue()
        }
        anchors.right: keyframeBtn.left
        anchors.rightMargin: 15
    }

    KeyframesButton {
        id: keyframeBtn
        visible: enableKeyframe
        onClicked: {
            keyframeToggled(checked)
        }
        anchors.right: parent.right
        anchors.rightMargin: 19
    }
}
