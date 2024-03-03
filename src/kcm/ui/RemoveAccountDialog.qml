/*
 *   SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

import org.kde.kaccounts as KAccounts

Kirigami.PromptDialog {
    id: component
    property int accountId
    property string displayName
    property string providerName
    signal accountRemoved()

    title: i18ndc("kaccounts-integration", "The title for a dialog which lets you remove an account", "Remove Account?")

    padding: Kirigami.Units.largeSpacing
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onAccepted: {
        var job = accountRemovalJob.createObject(component, { "accountId": component.accountId });
        job.start();
    }

    property var accountRemovalJob: Component {
        KAccounts.RemoveAccountJob {
            onFinished: component.accountRemoved()
        }
    }

    subtitle: {
        if (displayName.length > 0 && providerName.length > 0) {
            return i18ndc("kaccounts-integration", "The text for a dialog which lets you remove an account when both provider name and account name are available", "Are you sure you wish to remove the \"%1\" account \"%2\"?", providerName, displayName)
        } else if (displayName.length > 0) {
            return i18ndc("kaccounts-integration", "The text for a dialog which lets you remove an account when only the account name is available", "Are you sure you wish to remove the account \"%1\"?", displayName)
        } else {
            return i18ndc("kaccounts-integration", "The text for a dialog which lets you remove an account when only the provider name is available", "Are you sure you wish to remove this \"%1\" account?", providerName)
        }
    }
}
