/*
 *   Copyright 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

import QtQuick 2.12
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.12

import org.kde.kirigami 2.7 as Kirigami

import org.kde.kaccounts 1.2 as KAccounts

MessageBoxSheet {
    id: component
    title: i18nc("The title for a dialog which lets you set the human-readable name of an account", "Rename Account")
    property int accountId
    property string currentDisplayName
    signal accountRenamed()
    onSheetOpenChanged: {
        if (sheetOpen === true) {
            newAccountDisplayName.text = component.currentDisplayName;
        }
    }
    contentItem: Kirigami.FormLayout {
        Layout.preferredWidth: Kirigami.Units.gridUnit * 10
        Layout.margins: Kirigami.Units.largeSpacing
        Controls.TextField {
            id: newAccountDisplayName
            Kirigami.FormData.label: i18nc("Label for the text field used to enter a new human-readable name for an account", "Enter the new name of the account")
        }
    }
    actions: [
        Kirigami.Action {
            enabled: newAccountDisplayName.text.length > 0 && newAccountDisplayName.text !== component.currentDisplayName
            text: i18nc("Text of a button which will cause the human-readable name of an account to be set to a text specified by the user", "Set Account Name")
            onTriggered: {
                var job = accountDisplayNameJob.createObject(component, { "accountId": component.accountId, "displayName": newAccountDisplayName.text })
                job.start();
            }
            Component {
                id: accountDisplayNameJob
                KAccounts.ChangeAccountDisplayNameJob {
                    onFinished: component.accountRenamed()
                }
            }
        }
    ]
}
