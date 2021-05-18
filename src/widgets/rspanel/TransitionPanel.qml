import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import DFW.Components 1.0

import "./panelComponents/"
import "./indexPages/"

ToolPanelBase {
    id: transitionPanle
    
    tabModel: [qsTr("转      场")]

    contentItem: StackLayout {
        TransitionBasicPage {}
    }
}
