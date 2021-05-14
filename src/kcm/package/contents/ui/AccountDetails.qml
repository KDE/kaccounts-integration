/*
 *   SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
 *   SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12 as Controls
import QtQuick.Layouts 1.12

import org.kde.kirigami 2.14 as Kirigami
import org.kde.kcm 1.2

import org.kde.kaccounts 1.2 as KAccounts

ScrollViewKCM {
    id: component;

    title: i18nd("kaccounts-integration", "Account Details")

    property alias model: servicesList.model

    RemoveAccountDialog {
        id: accountRemover
        parent: component
        accountId: servicesList.model.accountId
        displayName: servicesList.model.accountDisplayName
        providerName: servicesList.model.accountProviderName
        onAccountRemoved: kcm.pop()
    }

    RenameAccountDialog {
        id: accountRenamer
        parent: component
        accountId: servicesList.model.accountId
        currentDisplayName: servicesList.model.accountDisplayName
    }

    Component {
        id: serviceToggleJob
        KAccounts.AccountServiceToggleJob { }
    }

    header: ColumnLayout {
        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.smallSpacing
            spacing: Kirigami.Units.smallSpacing
            Kirigami.Icon {
                source: model.accountIconName
                Layout.preferredWidth: Kirigami.Units.iconSizes.large
                Layout.preferredHeight: Kirigami.Units.iconSizes.large
            }
            Controls.Label {
                Layout.fillWidth: true
                text: {
                    if (model.accountDisplayName.length > 0 && model.accountProviderName.length > 0) {
                        return i18nd("kaccounts-integration", "%1 (%2)", model.accountDisplayName, model.accountProviderName)
                    } else if (model.accountDisplayName.length > 0) {
                        return model.accountDisplayName
                    } else {
                        return i18nd("kaccounts-integration", "%1 account", model.accountProviderName)
                    }
                }
            }
            Controls.ToolButton {
                icon.name: "edit-entry"
                Layout.preferredHeight: Kirigami.Units.iconSizes.large
                Layout.preferredWidth: Kirigami.Units.iconSizes.large
                onClicked: accountRenamer.open();
                Controls.ToolTip {
                    text: i18ndc("kaccounts-integration", "Button which spawns a dialog allowing the user to change the displayed account's human-readable name", "Change Account Display Name")
                }
            }
        }
        Kirigami.Heading {
            leftPadding: Kirigami.Units.smallSpacing
            visible: servicesList.count > 0
            level: 3
            text: i18ndc("kaccounts-integration", "Heading for a list of services available with this account", "Use This Account For")
        }
    }

    footer: RowLayout {
        Controls.Button {
            Layout.alignment: Qt.AlignRight
            text: i18nd("kaccounts-integration", "Remove This Account")
            icon.name: "edit-delete-remove"
            onClicked: accountRemover.open();
        }
    }

    view: ListView {
        id: servicesList

        clip: true

        ColumnLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.smallSpacing
            Controls.Label {
                visible: servicesList.count === 0
                Layout.fillWidth: true
                Layout.minimumHeight: component.height / 3
                enabled: false
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                text: i18ndc("kaccounts-integration", "A text shown when an account has no configurable services", "There are no configurable services available for this account. You can still change its display name by clicking the edit icon above.")
            }
        }

        delegate: Kirigami.CheckableListItem {
            icon: undefined
            text: model.displayName
            subtitle: model.description

            checked: model.enabled

            action: Controls.Action {
                onTriggered: {
                    checked = !checked
                    var job = serviceToggleJob.createObject(component, { "accountId": servicesList.model.accountId, "serviceId": model.name, "serviceEnabled": !model.enabled })
                job.start()
                }
            }
        }
    }
}
