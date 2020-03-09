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

import QtQuick 2.7
import QtQuick.Layouts 1.11

import org.kde.kirigami 2.7 as Kirigami

import org.kde.kaccounts 1.2 as KAccounts

MessageBoxSheet {
    id: component
    property int accountId
    property string displayName
    property string providerName
    signal accountRemoved()
    title: i18nc("The title for a dialog which lets you remove an account", "Remove Account?")
    text: {
        if (component.displayName.length > 0 && component.providerName.length > 0) {
            return i18nc("The text for a dialog which lets you remove an account when both provider name and account name are available", "Are you sure you wish to remove the \"%1\" account \"%2\"?", component.providerName, component.displayName)
        } else if (component.displayName.length > 0) {
            return i18nc("The text for a dialog which lets you remove an account when only the account name is available", "Are you sure you wish to remove the account \"%1\"?", component.displayName)
        } else {
            return i18nc("The text for a dialog which lets you remove an account when only the provider name is available", "Are you sure you wish to remove this \"%1\" account?", component.providerName)
        }
    }
    actions: [
        Kirigami.Action {
            text: i18nc("The label for a button which will cause the removal of a specified account", "Remove Account")
            onTriggered: {
                var job = accountRemovalJob.createObject(component, { "accountId": component.accountId });
                job.start();
            }
            Component {
                id: accountRemovalJob
                KAccounts.RemoveAccountJob {
                    onFinished: component.accountRemoved()
                }
            }
        }
    ]
}
