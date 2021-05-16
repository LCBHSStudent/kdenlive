import QtQuick.Controls 2.4
import QtQuick.Window 2.2
import Kdenlive.Controls 1.0
import QtQuick 2.11
import com.enums 1.0
import DFW.Components 1.0

Item {
    id: root
    objectName: "root"
    
    UIConfig { id: uiconfig }

    // default size, but scalable by user
    height: 300; width: 400
    property string clipName: controller.clipName
    property string markerText
    property int itemType: 0
    property point profile: controller.profile
    property double zoom
    property double scalex
    property double scaley
    // Zoombar properties
    // The start position of the zoomed area, between 0 and 1
    property double zoomStart: 0
    // The zoom factor (between 0 and 1). 0.5 means 2x zoom
    property double zoomFactor: 1
    // The pixel height of zoom bar, used to offset markers info
    property int zoomOffset: 0
    property bool showZoomBar: false
    property bool dropped: false
    property string fps: '-'
    property bool showMarkers: false
    property bool showTimecode: false
    property bool showFps: false
    property bool showSafezone: false
    // Display hover audio thumbnails overlay
    property bool showAudiothumb: false
    // Always display audio thumbs under video
    property bool permanentAudiothumb: false
    property bool showToolbar: false
    property real baseUnit: fontMetrics.font.pixelSize
    property int duration: 300
    property int mouseRulerPos: 0
    property double frameSize: 10
    property double timeScale: 1
    property int overlayType: controller.overlayType
    property color thumbColor1: controller.thumbColor1
    property color thumbColor2: controller.thumbColor2
    property color overlayColor: 'cyan'
    property bool isClipMonitor: true
    property int dragType: 0
    property int overlayMargin: (audioThumb.stateVisible && !audioThumb.isAudioClip && audioThumb.visible) ? (audioThumb.height + root.zoomOffset) : root.zoomOffset + (audioThumb.isAudioClip && audioSeekZone.visible) ? audioSeekZone.height : 0
    
    FontMetrics {
        id: fontMetrics
        font: fixedFont
    }

    Timer {
        id: thumbTimer
        interval: 3000; running: false;
    }

    signal editCurrentMarker()

    function updateScrolling()
    {
        if (thumbMouseArea.pressed) {
            var pos = Math.max(thumbMouseArea.mouseX, 0)
            pos += audioThumb.width/root.zoomFactor * root.zoomStart
            controller.setPosition(Math.min(pos / root.timeScale, root.duration));
            
        }
    }

    onDurationChanged: {
        clipMonitorRuler.updateRuler()
    }
    onWidthChanged: {
        clipMonitorRuler.updateRuler()
    }
    onClipNameChanged: {
        // Reset zoom on clip change
        root.zoomStart = 0
        root.zoomFactor = 1
        root.showZoomBar = false
        root.zoomOffset = 0

        // adjust monitor image size if audio thumb is displayed
        if (audioThumb.stateVisible && root.permanentAudiothumb && audioThumb.visible) {
            controller.rulerHeight = audioThumb.height + root.zoomOffset
        } else {
            controller.rulerHeight = root.zoomOffset
        }
    }
    
    onZoomOffsetChanged: {
        if (audioThumb.stateVisible && root.permanentAudiothumb && audioThumb.visible) {
            controller.rulerHeight = audioThumb.height + root.zoomOffset
        } else {
            controller.rulerHeight = root.zoomOffset
        }
    }
    
    onHeightChanged: {
        if (audioThumb.stateVisible && root.permanentAudiothumb && audioThumb.visible) {
            controller.rulerHeight = audioThumb.height + root.zoomOffset
        } else {
            controller.rulerHeight = root.zoomOffset
        }
    }

    function updatePalette() {
        clipMonitorRuler.forceRepaint()
    }

    function switchOverlay() {
        if (controller.overlayType >= 5) {
            controller.overlayType = 0
        } else {
            controller.overlayType = controller.overlayType + 1;
        }
        root.overlayType = controller.overlayType
    }

    MouseArea {
        id: barOverArea
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
        anchors.fill: parent
        onPositionChanged: {
            if (mouse.modifiers & Qt.ShiftModifier) {
                var pos = Math.max(mouseX, 0)
                pos += width/root.zoomFactor * root.zoomStart
                controller.setPosition(Math.min(pos / root.timeScale, root.duration));
            }
        }
        onWheel: {
            controller.seek(wheel.angleDelta.x + wheel.angleDelta.y, wheel.modifiers)
        }
        onEntered: {
            controller.setWidgetKeyBinding(i18n("<b>Click</b> to play, <b>Double click</b> for fullscreen, <b>Hover right</b> for toolbar, <b>Wheel</b> or <b>arrows</b> to seek, <b>Ctrl wheel</b> to zoom"));
        }
        onExited: {
            controller.setWidgetKeyBinding();
        }
    }

    SceneToolBar {
        id: sceneToolBar
        anchors {
            right: parent.right
            top: parent.top
            topMargin: 4
            rightMargin: 4
            leftMargin: 4
        }
    }

    Item {
        height: root.height - controller.rulerHeight
        width: root.width
        Item {
            id: frame
            objectName: "referenceframe"
            width: root.profile.x * root.scalex
            height: root.profile.y * root.scaley
            anchors.centerIn: parent
            anchors.verticalCenterOffset : (root.permanentAudiothumb && audioThumb.visible) ? -(audioThumb.height + root.zoomOffset) / 2 : -root.zoomOffset / 2

            Loader {
                width: controller.displayRect
                source: {
                    switch(root.overlayType)
                    {
                        case 0:
                            return '';
                        case 1:
                            return "OverlayStandard.qml";
                        case 2:
                            return "OverlayMinimal.qml";
                        case 3:
                            return "OverlayCenter.qml";
                        case 4:
                            return "OverlayCenterDiagonal.qml";
                        case 5:
                            return "OverlayThirds.qml";
                    }
                }
            }
        }
        Item {
            id: monitorOverlay
            anchors.fill: parent

            Item {
                id: audioThumb
                property bool stateVisible: (root.permanentAudiothumb || clipMonitorRuler.containsMouse || thumbMouseArea.containsMouse || dragZone.opacity == 1 || thumbTimer.running || root.showZoomBar)
                property bool isAudioClip: controller.clipType === ProducerType.Audio
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                    bottomMargin: root.zoomOffset
                }
                Label {
                    id: clipStreamLabel
                    font: fixedFont
                    anchors {
                        bottom: audioThumb.isAudioClip ? parent.bottom : parent.top
                        horizontalCenter: parent.horizontalCenter
                    }
                    color: "white"
                    text: controller.clipStream
                    background: Rectangle {
                        color: "#222277"
                    }
                    visible: text != ""
                    padding :4
                }
                height: isAudioClip ? parent.height : parent.height / 6
                //font.pixelSize * 3
                width: parent.width
                visible: (root.permanentAudiothumb || root.showAudiothumb) && (isAudioClip || controller.clipType === ProducerType.AV || controller.clipType === ProducerType.Playlist)
                onStateVisibleChanged: {
                    // adjust monitor image size
                    if (stateVisible && root.permanentAudiothumb && audioThumb.visible) {
                        controller.rulerHeight = audioThumb.height + root.zoomOffset
                    } else {
                        controller.rulerHeight = root.zoomOffset
                    }
                }

                states: [
                    State { when: audioThumb.stateVisible || audioThumb.isAudioClip;
                        PropertyChanges {   target: audioThumb; opacity: 1.0    } },
                    State { when: !audioThumb.stateVisible && !audioThumb.isAudioClip;
                        PropertyChanges {   target: audioThumb; opacity: 0.0    } }
                ]
                transitions: [ Transition {
                    NumberAnimation { property: "opacity"; duration: audioThumb.isAudioClip ? 0 : 500}
                } ]
                Rectangle {
                    color: "black"
                    opacity: audioThumb.isAudioClip || root.permanentAudiothumb ? 1 : 0.6
                    anchors.fill: parent
                }
                Rectangle {
                    color: "yellow"
                    opacity: 0.3
                    height: parent.height
                    x: controller.zoneIn * timeScale - (audioThumb.width/root.zoomFactor * root.zoomStart)
                    width: (controller.zoneOut - controller.zoneIn) * timeScale
                    visible: controller.zoneIn > 0 || controller.zoneOut < duration - 1
                }
                Repeater {
                    id: streamThumb
                    model: controller.audioStreams.length
                    onCountChanged: {
                        thumbTimer.start()
                    }
                    property double streamHeight: audioThumb.height / streamThumb.count
                    Item {
                        anchors.fill: parent
                        TimelineWaveform {
                            anchors.right: parent.right
                            anchors.left: parent.left
                            height: streamThumb.streamHeight
                            y: model.index * height
                            channels: controller.audioChannels[model.index]
                            binId: controller.clipId
                            audioStream: controller.audioStreams[model.index]
                            isFirstChunk: false
                            showItem: audioThumb.visible
                            format: controller.audioThumbFormat
                            normalize: controller.audioThumbNormalize
                            scaleFactor: audioThumb.width / (root.duration - 1) / root.zoomFactor
                            drawInPoint: 0
                            drawOutPoint: audioThumb.width
                            waveInPoint: (root.duration - 1) * root.zoomStart * channels
                            waveOutPointWithUpdate: (root.duration - 1) * (root.zoomStart + root.zoomFactor) * channels
                            fillColor1: root.thumbColor1
                            fillColor2: root.thumbColor2
                        }
                        Rectangle {
                            width: parent.width
                            y: (model.index + 1) * streamThumb.streamHeight
                            height: 1
                            visible: streamThumb.count > 1 && model.index < streamThumb.count - 1
                            color: 'yellow'
                        }
                    }
                }
                Rectangle {
                    color: "red"
                    width: 1
                    height: parent.height
                    x: controller.position * timeScale - (audioThumb.width/root.zoomFactor * root.zoomStart)
                }
                MouseArea {
                    id: thumbMouseArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    hoverEnabled: true
                    propagateComposedEvents: true
                    onPressed: {
                        if (audioThumb.isAudioClip && mouseY < audioSeekZone.y) {
                            mouse.accepted = false
                            return
                        }
                        var pos = Math.max(mouseX, 0)
                        pos += audioThumb.width/root.zoomFactor * root.zoomStart
                        controller.setPosition(Math.min(pos / root.timeScale, root.duration));
                    }
                    onPositionChanged: {
                        if (!(mouse.modifiers & Qt.ShiftModifier) && audioThumb.isAudioClip && mouseY < audioSeekZone.y) {
                            mouse.accepted = false
                            return
                        }
                        if (mouse.modifiers & Qt.ShiftModifier || pressed) {
                            var pos = Math.max(mouseX, 0)
                            pos += audioThumb.width/root.zoomFactor * root.zoomStart
                            controller.setPosition(Math.min(pos / root.timeScale, root.duration));
                        }
                    }
                    onWheel: {
                        if (wheel.modifiers & Qt.ControlModifier) {
                            if (wheel.angleDelta.y < 0) {
                                // zoom out
                                clipMonitorRuler.zoomOutRuler(wheel.x)
                            } else {
                                // zoom in
                                clipMonitorRuler.zoomInRuler(wheel.x)
                            }
                        } else {
                            wheel.accepted = false
                        }
                        
                    }
                    Rectangle {
                        id: audioSeekZone
                        width: parent.width
                        height: parent.height / 6
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: audioThumb.isAudioClip ? parent.height * 5 / 12 : 0
                        visible: audioThumb.isAudioClip && thumbMouseArea.containsMouse && thumbMouseArea.mouseY > y
                        color: 'yellow'
                        opacity: 0.5
                        Rectangle {
                            width: parent.width
                            height: 1
                            color: '#000'
                            anchors.top: parent.top
                        }
                        // frame ticks
                        Repeater {
                            id: rulerAudioTicks
                            model: parent.width / root.frameSize + 2
                            Rectangle {
                                x: index * root.frameSize - (clipMonitorRuler.rulerZoomOffset % root.frameSize)
                                anchors.top: audioSeekZone.top
                                height: (index % 5) ? audioSeekZone.height / 6 : audioSeekZone.height / 3
                                width: 1
                                color: '#000'
                                opacity: 0.8
                            }
                        }
                    }
                }
            }

            Label {
                id: timecode
                font.family: fontMetrics.font.family
                font.pointSize: 1.5 * fontMetrics.font.pointSize
                objectName: "timecode"
                color: "#ffffff"
                padding: 2
                background: Rectangle {
                    color: "#66000000"
                }
                text: controller.timecode
                visible: root.showTimecode
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    bottomMargin: overlayMargin
                }
            }
            Label {
                id: fpsdropped
                font.family: fontMetrics.font.family
                font.pointSize: 1.5 * fontMetrics.font.pointSize
                objectName: "fpsdropped"
                color: "#ffffff"
                padding: 2
                background: Rectangle {
                    color: root.dropped ? "#99ff0000" : "#66004400"
                }
                text: i18n("%1fps", root.fps)
                visible: root.showFps
                anchors {
                    right: timecode.visible ? timecode.left : parent.right
                    bottom: parent.bottom
                    bottomMargin: overlayMargin
                }
            }
            Label {
                id: labelSpeed
                font: fixedFont
                anchors {
                    left: parent.left
                    top: parent.top
                }
                visible: Math.abs(controller.speed) > 1
                text: "x" + controller.speed
                color: "white"
                background: Rectangle {
                    color: "darkgreen"
                }
                padding: 5
                horizontalAlignment: TextInput.AlignHCenter
            }
            Label {
                id: inPoint
                font: fixedFont
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                    bottomMargin: overlayMargin
                }
                visible: root.showMarkers && controller.position == controller.zoneIn
                text: i18n("In Point")
                color: "white"
                background: Rectangle {
                    color: "#228b22"
                }
                padding:4
                horizontalAlignment: TextInput.AlignHCenter
            }
            Label {
                id: outPoint
                font: fixedFont
                anchors {
                    left: inPoint.visible ? inPoint.right : parent.left
                    bottom: parent.bottom
                    bottomMargin: overlayMargin
                }
                visible: root.showMarkers && controller.position + 1 == controller.zoneOut
                text: i18n("Out Point")
                color: "white"
                background: Rectangle {
                    color: "#770000"
                }
                padding: 4
                horizontalAlignment: TextInput.AlignHCenter
            }
            TextField {
                id: marker
                font: fixedFont
                objectName: "markertext"
                activeFocusOnPress: true
                text: controller.markerComment
                onEditingFinished: {
                    root.markerText = marker.displayText
                    marker.focus = false
                    root.editCurrentMarker()
                }
                anchors {
                    left: outPoint.visible ? outPoint.right : inPoint.visible ? inPoint.right : parent.left
                    bottom: parent.bottom
                    bottomMargin: overlayMargin
                }
                visible: root.showMarkers && text != ""
                height: inPoint.height
                width: fontMetrics.boundingRect(displayText).width + 10
                horizontalAlignment: displayText == text ? TextInput.AlignHCenter : TextInput.AlignLeft
                background: Rectangle {
                    color: controller.markerColor
                }
                color: "#000"
                padding: 0
                maximumLength: 20
            }
        }

        Rectangle {
            // Audio or video only drag zone
            id: dragZone
            x: 2
            y: inPoint.visible || outPoint.visible || marker.visible ? parent.height - inPoint.height - height - 2 - overlayMargin : parent.height - height - 2 - overlayMargin
            width: childrenRect.width
            height: childrenRect.height
            color: Qt.rgba(uiconfig.foregroundColor.r,uiconfig.foregroundColor.g, uiconfig.foregroundColor.b, 0.7)
            radius: 4
            opacity: (dragAudioArea.containsMouse || dragVideoArea.containsMouse  || thumbMouseArea.containsMouse || (barOverArea.containsMouse && (barOverArea.mouseY >= (parent.height - inPoint.height - height - 2 - (audioThumb.height + root.zoomOffset) - root.baseUnit)))) ? 1 : 0
            visible: controller.clipHasAV
            onOpacityChanged: {
                if (opacity == 1) {
                    videoDragButton.x = 0
                    videoDragButton.y = 0
                    audioDragButton.x = videoDragButton.x + videoDragButton.width
                    audioDragButton.y = 0
                }
            }
            Row {
                id: dragRow
                ToolButton {
                    id: videoDragButton
                    icon.name: "kdenlive-show-video"
                    Drag.active: dragVideoArea.drag.active
                    Drag.dragType: Drag.Automatic
                    Drag.mimeData: {
                        "kdenlive/producerslist" : "V" + controller.clipId + "/" + controller.zoneIn + "/" + (controller.zoneOut - 1)
                    }
                    MouseArea {
                        id: dragVideoArea
                        hoverEnabled: true
                        anchors.fill: parent
                        propagateComposedEvents: true
                        cursorShape: Qt.PointingHand
                        drag.target: parent
                        onExited: {
                            parent.x = 0
                            parent.y = 0
                        }
                    }
                }
                ToolButton {
                    id: audioDragButton
                    icon.name: "audio-volume-medium"
                    Drag.active: dragAudioArea.drag.active
                    Drag.dragType: Drag.Automatic
                    Drag.mimeData: {
                        "kdenlive/producerslist" : "A" + controller.clipId + "/" + controller.zoneIn + "/" + (controller.zoneOut - 1)
                    }
                    MouseArea {
                        id: dragAudioArea
                        hoverEnabled: true
                        anchors.fill: parent
                        propagateComposedEvents: true
                        cursorShape: Qt.PointingHand
                        drag.target: parent
                        onExited: {
                            parent.x = videoDragButton.x + videoDragButton.width
                            parent.y = 0
                        }
                    }
                }
            }
        }
    }
    MonitorRuler {
        id: clipMonitorRuler
        anchors {
            left: root.left
            right: root.right
            bottom: root.bottom
        }
        height: controller.rulerHeight
    }
    
    Rectangle {
        color: uiconfig.darkerSpaceColor
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 1
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: 175
    }
}
