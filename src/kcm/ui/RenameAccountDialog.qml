/*
 *   SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.kaccounts as KAccounts

MessageBoxSheet {
    id: component
    title: i18ndc("kaccounts-integration", "The title for a dialog which lets you set the human-readable name of an account", "Rename Account")
    property int accountId
    property string currentDisplayName
    signal accountRenamed()
    onVisibleChanged: {
        if (sheetOpen === true) {
            newAccountDisplayName.text = currentDisplayName;
        }
    }
    contentItem: Kirigami.FormLayout {
        Layout.preferredWidth: Kirigami.Units.gridUnit * 10
        Layout.margins: Kirigami.Units.largeSpacing
        Controls.TextField {
            id: newAccountDisplayName
            Kirigami.FormData.label: i18ndc("kaccounts-integration", "Label for the text field used to enter a new human-readable name for an account", "Enter new name:")
        }
    }
    Component {
        id: accountDisplayNameJob
        KAccounts.ChangeAccountDisplayNameJob {
            onFinished: component.accountRenamed()
        }
    }
    actions: [
        Kirigami.Action {
            enabled: newAccountDisplayName.text.length > 0 && newAccountDisplayName.text !== component.currentDisplayName
            text: i18ndc("kaccounts-integration", "Text of a button which will cause the human-readable name of an account to be set to a text specified by the user", "Set Account Name")
            onTriggered: {
                var job = accountDisplayNameJob.createObject(component, { "accountId": component.accountId, "displayName": newAccountDisplayName.text })
                job.start();
            }
        }
    ]
}
