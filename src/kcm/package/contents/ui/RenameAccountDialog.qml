/*
 *   SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
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
            newAccountDisplayName.text = currentDisplayName;
        }
    }
    contentItem: ColumnLayout {
        Layout.preferredWidth: Kirigami.Units.gridUnit * 10
        Layout.margins: Kirigami.Units.largeSpacing
        Controls.Label {
            text: i18nc("Label for the text field used to enter a new human-readable name for an account", "Enter the new name of the account")
        }
        Controls.TextField {
            id: newAccountDisplayName
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
