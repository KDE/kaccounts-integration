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

Kirigami.PromptDialog {
    id: component
    title: i18ndc("kaccounts-integration", "The title for a dialog which lets you set the human-readable name of an account", "Rename Account")
    property int accountId
    property string currentDisplayName
    signal accountRenamed()
    onVisibleChanged: {
        if (visible === true) {
            newAccountDisplayName.text = currentDisplayName;
        }
    }

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onAccepted: {
        var job = accountDisplayNameJob.createObject(component, { "accountId": component.accountId, "displayName": newAccountDisplayName.text })
        job.start();
    }

    Kirigami.FormLayout {
        Controls.TextField {
            id: newAccountDisplayName
            Kirigami.FormData.label: i18ndc("kaccounts-integration", "Label for the text field used to enter a new human-readable name for an account", "Enter new name:")
        }
    }
    property var accountDisplayNameJob: Component {
        KAccounts.ChangeAccountDisplayNameJob {
            onFinished: component.accountRenamed()
        }
    }
}
