import QtQuick 2.12
import DFW.Components 1.0

/**
    @anotation: sectionContent中的组件最好保持width: parent.width & height: 43px
                简易封装于SectionContentItem.qml
 */

Item {
    id: blockRoot

    anchors.horizontalCenter: parent.horizontalCenter
    width: parent.width - 40
    height: sectionCuttler.checked? /* 15 + 20 + 20 */ 55 + contentBg.height: 50

    property alias section: sectionText.text
    property alias content: contentBg.sectionContent
    property string serviceName: ""
    property int    serviceEnum: -1
    property bool serviceSelected: false


    ThemeText {
        id: sectionText
        anchors {
            top: parent.top
        }
    }

    IconToolButton {
        id: sectionCuttler
        width: 20; height: 20
        iconSource: 'qrc:/icons/dark/toolpanel/cuttler.png'
        checkable: true
        rotation: checked? 0: 90
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
        border.color: "#FF707070"
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
            if (!serviceSelected) {
                if (attFilterModel.isProducerSelected) {
                    var index = attFilterModel.findServiceIndex(serviceName)
                    var isNew = false
                    if (index < 0) {
                        attFilterModel.add(metadataModel.specifiedFiler(serviceName))
                        index = attFilterModel.findServiceIndex(serviceName)
                    }

                    filterCon.setCurrentFilter(index, isNew, serviceEnum)
                    serviceSelected = true
                }
            }
            mouse.accepted = false
        }
    }
}
