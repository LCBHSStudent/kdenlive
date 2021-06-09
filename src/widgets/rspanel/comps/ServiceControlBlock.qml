import QtQuick 2.12
import DFW.Components 1.0

/**
    @anotation: sectionContent中的组件最好保持width: parent.width & height: 43px
                简易封装于SectionContentItem.qml
 */

Item {
    id: blockRoot

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.horizontalCenterOffset: -1
    width: parent.width - 42
    // top margin: 32
    height: sectionCuttler.checked? /* 15 + 20 + 20 */ 55 + contentBg.height: 50

    property alias  section: sectionText.text
    property alias  content: contentBg.sectionContent
    property string serviceName: ""
    property bool   serviceSelected: false
    
    signal refreshParams(var kv)

    ThemeText {
        id: sectionText
        color: uiconfig.lighterFontColor
        anchors {
            top: parent.top
        }
    }

    IconToolButton {
        id: sectionCuttler
        width: 20; height: 20
        iconSource: 'qrc:/classic/controllers/indicator-downarrow.png'
        checkable: true
        rotation: checked? 0: -90
        bgColor: "transparent"
        anchors {
            left: parent.left
            leftMargin: 88
            verticalCenter: sectionText.verticalCenter
        }
    }

    Rectangle {
        id: contentBg
        width: parent.width
        visible: sectionCuttler.checked
        height: sectionContent.height
        anchors {
            top: sectionText.bottom
            topMargin: 15
        }

        color: "#00000000"
        border.color: "#494950"
        border.width: 0.5

        property Column sectionContent: null

        onSectionContentChanged: {
            sectionContent.parent = this
            sectionContent.width = Qt.binding(function() { return width })
            sectionContent.anchors.centerIn = this
            sectionContent.anchors.left = this.left
            sectionContent.anchors.top = this.top
        }
    }

    MouseArea {
        anchors.fill: parent
        propagateComposedEvents: true

        onPressed: {
            mouse.accepted = false
            serviceSelected = true
        }
    }
    
    onServiceSelectedChanged: {
        if (serviceSelected) {
            console.debug(assetCtrl.selectService(serviceName))
        }
    }
    
    Connections {
        target: assetCtrl
        function onRefreshParams(serviceId, kv) {
            if (serviceId === serviceName) {
                blockRoot.refreshParams(kv)
            }
        }
    }
}
