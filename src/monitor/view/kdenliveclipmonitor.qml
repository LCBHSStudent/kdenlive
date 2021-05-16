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
    
    property string baseThumbPath: controller.clipId >= 0? 'image://thumbnail/' + controller.clipId + '/' + documentId + '/#': ""
    
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
        update()
    }
    onWidthChanged: {
        update()
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
        
    }
    
    Rectangle {
        color: uiconfig.darkerSpaceColor
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: 175
    }
    
    Rectangle {
        id: videoEditBar
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 93
        }

        width: parent.width - 8
        height: 62
        color: uiconfig.alphaColor(uiconfig.foregroundColor, 0.2)
        border.width: 1
        border.color: uiconfig.foregroundColor
        
        Rectangle {
            x: inControl.x + inControl.width / 2
            width: outControl.x - inControl.x 
            height: parent.height
            color: uiconfig.foregroundColor
        }
        
        Row {
            clip: true
            height: 36
            width: parent.width - 2
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 5
            }
            
            Repeater {
                id: thumbRepeater
                model: Math.floor(videoEditBar.width / 64) + 1
                
                delegate: Image {
                    height: parent.height
                    fillMode: Image.PreserveAspectFit
                    source: baseThumbPath + Math.floor(root.duration / thumbRepeater.model * modelData)
                }
            }
        }
        
        Repeater {
            id: streamThumb
            model: controller.audioStreams.length
            onCountChanged: {
                thumbTimer.start()
            }
            property double streamHeight: 19 / streamThumb.count
            Item {
                anchors.fill: parent
                anchors.topMargin: 2
                anchors.bottomMargin: 42
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
            }
        }
        
        Rectangle {
            id: inControl
            x: Math.min(1, controller.zoneIn / root.duration) * videoEditBar.width - width / 2
            width: 7
            height: parent.height + 8
            anchors.verticalCenter: parent.verticalCenter
            color: inControlArea.pressed? uiconfig.foregroundColor: uiconfig.lighterSpaceColor
            border.width: 2
            border.color: "#FFFFFF"
            Drag.active: true
            
            MouseArea {
                id: inControlArea
                anchors.fill: parent
                drag.target: parent
                drag.axis: Drag.XAxis
                drag.minimumX: -width / 2
                drag.maximumX: outControl.x
                cursorShape: Qt.PointingHandCursor
            
                onPressed: {
                    inControl.z = 999
                    outControl.z = 998
                }
            }
            
            onXChanged: {
                controller.qBlockSignals(true)
                controller.zoneIn = (x + width / 2) / videoEditBar.width * root.duration
                inControlLabelText.text = controller.toTimecode(controller.zoneIn)
                controller.qBlockSignals(false)
            }
            
            Connections {
                target: controller
                function onClipIdChanged() {
                    controller.zoneIn = (inControl.x + inControl.width / 2) / videoEditBar.width * root.duration
                    inControlLabelText.text = controller.toTimecode(controller.zoneIn)
                }
            }
        }
        
        Rectangle {
            id: outControl
            x: Math.min(1, controller.zoneOut / root.duration) * videoEditBar.width - width / 2
            width: 7
            height: parent.height + 8
            anchors.verticalCenter: parent.verticalCenter
            color: outControlArea.pressed? uiconfig.foregroundColor: uiconfig.lighterSpaceColor
            border.width: 2
            border.color: "#FFFFFF"
            Drag.active: true
            
            MouseArea {
                id: outControlArea
                anchors.fill: parent
                drag.target: parent
                drag.axis: Drag.XAxis
                drag.minimumX: inControl.x 
                drag.maximumX: videoEditBar.width - width / 2
                cursorShape: Qt.PointingHandCursor
                
                onPressed: {
                    inControl.z = 998
                    outControl.z = 999
                }
            }
            
            onXChanged: {
                console.debug(x, root.duration, controller.zoneIn, controller.zoneOut)
                controller.qBlockSignals(true)
                controller.zoneOut = (x + width / 2) / videoEditBar.width * root.duration
                outControlLabelText.text = controller.toTimecode(controller.zoneOut)
                controller.qBlockSignals(false)
            }
        }
        
        Rectangle {
            id: inControlLabel
            visible: root.visible
            z: inControl.z + 2
            width: 78
            height: 20
            anchors.left: inControl.right
            anchors.leftMargin: width + inControl.x + inControl.width > root.width? -inControl.width - width: 0            
            anchors.bottom: inControl.top
            anchors.bottomMargin: -2
            color: uiconfig.alphaColor(Qt.black, 0.4)
            
            ThemeText {
                id: inControlLabelText
                anchors.centerIn: parent
            }
        }
        
        Rectangle {
            id: outControlLabel
            visible: root.visible
            z: outControl.z + 2
            width: 78
            height: 20
            anchors.left: outControl.right
            anchors.leftMargin: width + outControl.x + outControl.width > root.width? -outControl.width - width: 0
            anchors.bottom: outControl.top
            anchors.bottomMargin: -2
            
            color: uiconfig.alphaColor(Qt.black, 0.4)
            
            ThemeText {
                id: outControlLabelText
                anchors.centerIn: parent
            }
        }
    }
    
    ThemeText {
        anchors {
            left: videoEditBar.left
            top: videoEditBar.bottom
            topMargin: 10
        }
        text: controller.timecode
    }
    
    ThemeText {
        anchors {
            right: videoEditBar.right
            top: videoEditBar.bottom
            topMargin: 10
        }
        text: controller.toTimecode(root.duration)
    }
    
    Row {
        id: editControl
        anchors.verticalCenter: playControl.verticalCenter
        anchors.right: playControl.left
        anchors.rightMargin: 30
        spacing: 20
        IconToolButton {
            size: 50   
            iconSource: "qrc:/classic/components/ptcontroller/mark.png"            
        }
        IconToolButton {
            size: 50     
            iconSource: "qrc:/classic/components/ptcontroller/speed_adjust.png"
        }
    }
    
    Row {
        id: playControl
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 23
        anchors.horizontalCenter: parent.horizontalCenter
        IconToolButton {
            size: 50          
            iconSource: "qrc:/classic/components/ptcontroller/prev_keypoint.png"
        }
        IconToolButton {
            size: 50   
            iconSource: "qrc:/classic/components/ptcontroller/prev_frame.png"
        }
        IconToolButton {
            size: 50   
            iconSource: "qrc:/classic/components/ptcontroller/play.png"
        }
        IconToolButton {
            size: 50   
            iconSource: "qrc:/classic/components/ptcontroller/next_frame.png"
        }
        IconToolButton {
            size: 50         
            iconSource: "qrc:/classic/components/ptcontroller/next_keypoint.png"
        }
    }
    
    IconToolButton {
        size: 50      
        
        anchors.verticalCenter: playControl.verticalCenter
        anchors.left: playControl.right
        anchors.leftMargin: 30
        
        iconSource: "qrc:/classic/controllers/btn_add_to_track.png"
    }
    
    
    
    
    // former audio overlay
    Item {
        id: monitorOverlay
        anchors.fill: parent

        Item {
            id: audioThumb
            property bool stateVisible: (root.permanentAudiothumb || thumbTimer.running || root.showZoomBar)
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

            
            // 指针
//            Rectangle {
//                color: "red"
//                width: 1
//                height: parent.height
//                x: controller.position * timeScale - (audioThumb.width/root.zoomFactor * root.zoomStart)
//            }
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
                            x: index * root.frameSize - root.frameSize
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
    
}
