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

import org.kde.kaccounts 1.2 as KAccounts
import org.kde.kcm 1.2

ScrollViewKCM {
    id: kaccountsRoot

    // Existing accounts
    view: ListView {
        model: KAccounts.AccountsModel { }

        delegate: Kirigami.SwipeListItem {
            id: accountDelegate
            width: ListView.view.width

            contentItem: RowLayout {
                implicitWidth: accountDelegate.ListView.view.width
                implicitHeight: Kirigami.Units.iconSizes.large + Kirigami.Units.smallSpacing * 2
                spacing: Kirigami.Units.smallSpacing
                Kirigami.Icon {
                    source: model.iconName
                    Layout.preferredWidth: Kirigami.Units.iconSizes.large
                    Layout.preferredHeight: Kirigami.Units.iconSizes.large
                }
                Controls.Label {
                    Layout.fillWidth: true
                    text: {
                        if (model.displayName.length > 0 && model.providerName.length > 0) {
                            return i18n("%1 (%2)", model.displayName, model.providerName)
                        } else if (model.displayName.length > 0) {
                            return model.displayName
                        } else {
                            return i18n("%1 account", model.providerName)
                        }
                    }
                }
            }
            actions: [
                Kirigami.Action {
                    text: i18nc("Tooltip for an action which will offer the user to remove the mentioned account", "Remove %1", accountDelegate.contentItem.text)
                    iconName: "edit-delete-remove"
                    onTriggered: {
                        accountRemovalDlg.accountId = model.id;
                        accountRemovalDlg.displayName = model.displayName;
                        accountRemovalDlg.providerName = model.providerName;
                        accountRemovalDlg.open();
                    }
                }
            ]
            onClicked: kcm.push("AvailableServices.qml", {model: model.services})
        }
        Kirigami.AbstractCard {
            visible: view.count === 0
            anchors.centerIn: parent
            width: kaccountsRoot.width * 2/ 3
            height: kaccountsRoot.height * 2 / 3
            header: Kirigami.Heading {
                Layout.fillWidth: true;
                text: i18n("No accounts yet")
            }
            contentItem: Controls.Label {
                clip: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                textFormat: Text.RichText
                text: i18nc("A text shown when a user has not yet added any accounts", "You have not added any accounts yet.<br /><br />Click on <a href=\"add-new-account\">\"Add New Account\"</a> to do so.")
                onLinkActivated: kcm.push("AvailableAccounts.qml")
            }
        }
    }

    MessageBoxSheet {
        id: accountRemovalDlg
        parent: kaccountsRoot
        property int accountId
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
                onTriggered: {
                    var job = accountRemovalJob.createObject(kaccountsRoot, { "accountId": accountRemovalDlg.accountId });
                    job.start();
                }
            }
        ]
    }

    Component {
        id: accountRemovalJob
        KAccounts.RemoveAccount { }
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
