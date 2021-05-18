import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "./comps/"
import "./pages/"

// TODO: **! add composited filter controller group
ToolPanelBase {
    id: videoPanle

    anchors.fill: parent
    
    tabModel: [qsTr("基  本"), qsTr("调  色")]

    contentItem: StackLayout {
        VideoBasicPage {}
        VideoColorPage {}
    }
}
