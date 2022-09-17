/*
 *   SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Layouts 1.12 as QtLayouts

import org.kde.kirigami 2.7 as Kirigami

Kirigami.OverlaySheet {
    id: component

    property alias title: headerLabel.text
    property alias text: messageLabel.text
    property list<QtObject> actions

    showCloseButton: true
    header: Kirigami.Heading {
        id: headerLabel
        QtLayouts.Layout.fillWidth: true
        elide: Text.ElideRight
    }
    contentItem: QtControls.Label {
        id: messageLabel
        QtLayouts.Layout.preferredWidth: Kirigami.Units.gridUnit * 10
        QtLayouts.Layout.margins: Kirigami.Units.largeSpacing
        wrapMode: Text.Wrap
    }
    footer: QtLayouts.RowLayout {
        Item { QtLayouts.Layout.fillWidth: true }
        Repeater {
            model: component.actions;
            QtControls.Button {
                action: modelData
                Connections {
                    target: action
                    function onTriggered() { component.close(); }
                }
            }
        }
    }
}
