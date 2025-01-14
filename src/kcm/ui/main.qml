/*
 *   SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *   SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM

import org.kde.kaccounts as KAccounts

KCM.ScrollViewKCM {
    id: kaccountsRoot

    implicitHeight: Kirigami.Units.gridUnit * 28
    implicitWidth: Kirigami.Units.gridUnit * 28

    actions: [
        Kirigami.Action {
            text: i18ndc("kaccounts-integration", "@action:button", "Add Account…")
            icon.name: "list-add-symbolic"
            onTriggered: kcm.push("AvailableAccounts.qml")
        }
    ]

    // Existing accounts
    view: ListView {

        clip: true

        model: KAccounts.AccountsModel { }

        delegate: Controls.ItemDelegate {
            id: delegate
            width: ListView.view.width

            text: {
                if (model.displayName.length > 0 && model.providerName.length > 0) {
                    return i18nd("kaccounts-integration", "%1 (%2)", model.displayName, model.providerDisplayName)
                } else if (model.displayName.length > 0) {
                    return model.displayName
                } else {
                    return i18nd("kaccounts-integration", "%1 account", model.providerName)
                }
            }
            icon.name: model.iconName

            contentItem: RowLayout {

                spacing: Kirigami.Units.smallSpacing

                Kirigami.IconTitleSubtitle {
                    Layout.fillWidth: true

                    title: delegate.text
                    icon: icon.fromControlsIcon(delegate.icon)
                    selected: delegate.highlighted
                    font: delegate.font
                }

                Controls.ToolButton {
                    text: i18ndc("kaccounts-integration", "Tooltip for an action which will offer the user to remove the mentioned account", "Remove %1", delegate.contentItem.text)
                    icon.name: "edit-delete-remove"
                    display: Controls.ToolButton.IconOnly
                    onClicked: {
                        accountRemover.accountId = model.id;
                        accountRemover.displayName = model.displayName;
                        accountRemover.providerName = model.providerName;
                        accountRemover.open();
                    }
                }
            }

            onClicked: kcm.push("AccountDetails.qml", {model: model.services})
        }

        Kirigami.PlaceholderMessage {
            visible: view.count === 0
            anchors.centerIn: parent
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
            icon.name: "internet-services"
            text: i18ndc("kaccounts-integration", "A text shown when a user has not yet added any accounts", "No accounts added yet")
            explanation: xi18ndc("kaccounts-integration", "@info", "Click <interface>Add Account…</interface> to add one")

        }
    }

    RemoveAccountDialog {
        id: accountRemover
        parent: kaccountsRoot
    }
}
