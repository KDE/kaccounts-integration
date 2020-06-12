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

import QtQuick 2.12
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.12

import org.kde.kirigami 2.12 as Kirigami
import org.kde.kcm 1.2

import org.kde.kaccounts 1.2 as KAccounts

ScrollViewKCM {
    id: kaccountsRoot

    // Existing accounts
    view: ListView {

        clip: true

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
                        accountRemover.accountId = model.id;
                        accountRemover.displayName = model.displayName;
                        accountRemover.providerName = model.providerName;
                        accountRemover.open();
                    }
                }
            ]
            onClicked: kcm.push("AccountDetails.qml", {model: model.services})
        }

        Kirigami.PlaceholderMessage {
            visible: view.count === 0
            anchors.centerIn: parent
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
            text: i18nc("A text shown when a user has not yet added any accounts", "No accounts added yet")

            helpfulAction: Kirigami.Action {
                text: i18n("Add New Account...")
                icon.name: "contact-new"
                onTriggered: kcm.push("AvailableAccounts.qml")
            }
        }
    }

    RemoveAccountDialog {
        id: accountRemover
        parent: kaccountsRoot
    }

    footer: RowLayout {
        Controls.Button {
            visible: !noAccountsLayout.visible
            Layout.alignment: Qt.AlignRight
            text: i18n("Add New Account...")
            icon.name: "contact-new"
            onClicked: kcm.push("AvailableAccounts.qml")
        }
    }
}
