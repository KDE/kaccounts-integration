/*
 *   Copyright 2015 Martin Klapetek <mklapetek@kde.org>
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
import QtQuick.Controls 2.0 as Controls

import org.kde.kirigami 2.7 as Kirigami

import org.kde.kaccounts 1.0
import org.kde.kcm 1.2

import Ubuntu.OnlineAccounts 0.1 as OA

ScrollViewKCM {
    id: kaccountsRoot

    // Existing accounts
    view: ListView {
        model: OA.AccountServiceModel {
            id: accountsModel
            service: "global"
            includeDisabled: true
        }

        delegate: Kirigami.SwipeListItem {
            id: accountDelegate
            width: ListView.view.width

            contentItem: Controls.Label {
                text: {
                    if (model.displayName.length > 0 && model.providerName.length > 0) {
                        return i18n("%1 (%2)", model.displayName, model.providerName)
                    } else if (model.displayName.length > 0) {
                        return model.displayName
                    } else {
                        return i18n("%1 account", model.providerName)
                    }
                }

                OA.Account {
                    id: account
                    objectHandle: model.accountHandle
                }
            }
            actions: [
                Kirigami.Action {
                    text: i18nc("Tooltip for an action which will offer the user to remove the mentioned account", "Remove %1", accountDelegate.contentItem.text)
                    iconName: "edit-delete-remove"
                    onTriggered: {
                        accountRemovalDlg.account = account;
                        accountRemovalDlg.displayName = model.displayName;
                        accountRemovalDlg.providerName = model.providerName;
                        accountRemovalDlg.open();
                    }
                }
            ]
            onClicked: kcm.push("AvailableServices.qml", {accountId: model.accountId})
        }
        Controls.Label {
            anchors {
                fill: parent
                margins: Kirigami.Units.largeSpacing
            }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            visible: parent.count === 0
            opacity: 0.5
            text: i18nc("A text shown when a user has not yet added any accounts", "You have not added any accounts yet.\nClick the \"Add new Account\" button below to do so.")
        }
    }
    MessageBoxSheet {
        id: accountRemovalDlg
        parent: kaccountsRoot
        property QtObject account
        property string displayName
        property string providerName
        title: i18nc("The title for a dialog which lets you remove an account", "Remove Account?")
        text: {
            if (accountRemovalDlg.displayName.length > 0 && accountRemovalDlg.providerName.length > 0) {
                return i18nc("The text for a dialog which lets you remove an account when both provider name and account name are available", "Are you sure you wish to remove the \"%1\" account \"%2\"?", accountRemovalDlg.providerName, accountRemovalDlg.displayName)
            } else if (accountRemovalDlg.displayName.length > 0) {
                return i18nc("The text for a dialog which lets you remove an account when only the account name is available", "Are you sure you wish to remove the account \"%1\"?", accountRemovalDlg.displayName)
            } else {
                return i18nc("The text for a dialog which lets you remove an account when only the provider name is available", "Are you sure you wish to remove this \"%1\" account?", accountRemovalDlg.providerName)
            }
        }
        actions: [
            Kirigami.Action {
                text: i18nc("The label for a button which will cause the removal of a specified account", "Remove Account")
                onTriggered: { accountRemovalDlg.account.remove(); }
            }
        ]
    }

    footer: RowLayout {
        Controls.Button {
            Layout.alignment: Qt.AlignRight
            text: i18n("Add new Account")
            icon.name: "contact-new"
            onClicked: kcm.push("AvailableAccounts.qml")
        }
    }
}
