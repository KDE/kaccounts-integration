/*
 *   SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *   SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.12

import org.kde.kirigami 2.12 as Kirigami
import org.kde.kcm 1.2

import org.kde.kaccounts 1.2 as KAccounts

ScrollViewKCM {
    id: kaccountsRoot

    implicitHeight: Kirigami.Units.gridUnit * 28
    implicitWidth: Kirigami.Units.gridUnit * 28

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
                            return i18nd("kaccounts-integration", "%1 (%2)", model.displayName, model.providerDisplayName)
                        } else if (model.displayName.length > 0) {
                            return model.displayName
                        } else {
                            return i18nd("kaccounts-integration", "%1 account", model.providerName)
                        }
                    }
                }
            }
            actions: [
                Kirigami.Action {
                    text: i18ndc("kaccounts-integration", "Tooltip for an action which will offer the user to remove the mentioned account", "Remove %1", accountDelegate.contentItem.text)
                    icon.name: "edit-delete-remove"
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
            text: i18ndc("kaccounts-integration", "A text shown when a user has not yet added any accounts", "No accounts added yet")
            explanation: xi18ndc("kaccounts-integration", "@info", "Click the <interface>Add New Account...</interface> button below to add one")
        }
    }

    RemoveAccountDialog {
        id: accountRemover
        parent: kaccountsRoot
    }

    footer: RowLayout {
        Controls.Button {
            Layout.alignment: Qt.AlignRight
            text: i18nd("kaccounts-integration", "Add New Account...")
            icon.name: "contact-new"
            onClicked: kcm.push("AvailableAccounts.qml")
        }
    }
}
