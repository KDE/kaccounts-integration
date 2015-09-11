/*
 *   Copyright 2015 (C) Martin Klapetek <mklapetek@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

ColumnLayout {
    id: basicInfoLayout
    property bool canContinue: helper.isServerValid && nameText.text.length > 0 && passwordText.text.length > 0

    Timer {
        id: checkServerTimer
        interval: 1000
        repeat: false
        running: false

        onTriggered: {
            helper.checkServer(nameText.text, passwordText.text, serverText.text);
        }
    }

    PlasmaComponents.TextField {
        id: nameText
        Layout.fillWidth: true
        clearButtonShown: true
        placeholderText: "Username"
    }

    PlasmaComponents.TextField {
        id: passwordText
        Layout.fillWidth: true
        clearButtonShown: true
        placeholderText: "Password"
        echoMode: TextInput.Password
    }

    PlasmaComponents.TextField {
        id: serverText
        Layout.fillWidth: true
        clearButtonShown: true
        placeholderText: "Server"

        onTextChanged: checkServerTimer.restart();
    }

    PlasmaComponents.Label {
        id: errorLabel
        Layout.fillWidth: true
        visible: text.length > 0 && !checkServerTimer.running
        text: helper.errorMessage
        wrapMode: Text.WordWrap
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true

        PlasmaComponents.BusyIndicator {
            id: busy
            width: parent.height > parent.width ? Math.round(parent.width/2) : Math.round(parent.height/2)
            height: width
            anchors.centerIn: parent
            running: helper.isWorking
            visible: running
        }

        PlasmaCore.IconItem {
            width: busy.width
            height: width
            anchors.centerIn: parent
            source: "dialog-ok"
            visible: !helper.isWorking && helper.isServerValid && !errorLabel.visible
        }
    }
}
