/*
 *   Copyright 2019 Nicolas Fella <nicolas.fella@gmx.de>
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
import org.kde.kirigami 2.4 as Kirigami
import org.kde.kaccounts 1.2 as KAccounts
import org.kde.kcm 1.2

SimpleKCM {
    id: component;

    title: i18n("Account Details")

    property alias model: servicesList.model

    MessageBoxSheet {
        id: accountRemovalDlg
        parent: component
        property string displayName: servicesList.model.accountDisplayName
        property string providerName: servicesList.model.accountProviderName
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
                    var job = accountRemovalJob.createObject(component, { "accountId": servicesList.model.accountId });
                    job.start();
                }
            }
        ]
    }

    MessageBoxSheet {
        id: renameAccountDlg
        parent: component
        title: i18nc("The title for a dialog which lets you set the human-readable name of an account", "Rename Account")
        onSheetOpenChanged: {
            if (sheetOpen === true) {
                newAccountDisplayName.text = servicesList.model.accountDisplayName;
            }
        }
        contentItem: Kirigami.FormLayout {
            Layout.preferredWidth: Kirigami.Units.gridUnit * 10
            Layout.margins: Kirigami.Units.largeSpacing
            Controls.TextField {
                id: newAccountDisplayName
                Kirigami.FormData.label: i18nc("Label for the text field used to enter a new human-readable name for an account", "Enter the new name of the account")
            }
        }
        actions: [
            Kirigami.Action {
                enabled: newAccountDisplayName.text.length > 0
                text: i18nc("Text of a button which will cause the human-readable name of an account to be set to a text specified by the user", "Set Account Name")
                onTriggered: {
                    var job = accountDisplayNameJob.createObject(component, { "accountId": servicesList.model.accountId, "displayName": newAccountDisplayName.text })
                    job.start();
                }
            }
        ]
    }

    Component {
        id: accountDisplayNameJob
        KAccounts.ChangeAccountDisplayNameJob { }
    }

    Component {
        id: serviceToggleJob
        KAccounts.AccountServiceToggleJob { }
    }

    Component {
        id: accountRemovalJob
        KAccounts.RemoveAccountJob {
            onFinished: {
                kcm.pop();
            }
        }
    }

    header: RowLayout {
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
                    return i18n("%1 (%2)", model.accountDisplayName, model.accountProviderName)
                } else if (model.accountDisplayName.length > 0) {
                    return model.accountDisplayName
                } else {
                    return i18n("%1 account", model.accountProviderName)
                }
            }
        }
        Controls.ToolButton {
            icon.name: "edit-entry"
            display: Controls.AbstractButton.IconOnly
            Layout.preferredHeight: Kirigami.Units.iconSizes.large
            Layout.preferredWidth: Kirigami.Units.iconSizes.large
            onClicked: renameAccountDlg.open();
            Controls.ToolTip {
                visible: parent.hovered && !parent.pressed
                text: i18nc("Button which spawns a dialog allowing the user to change the displayed account's human-readable name", "Change Account Display Name")
                delay: Kirigami.Units.toolTipDelay
                timeout: 5000
                y: parent.height
            }
        }
    }

    footer: RowLayout {
        Controls.Button {
            Layout.alignment: Qt.AlignRight
            text: i18n("Remove This Account")
            icon.name: "edit-delete-remove"
            onClicked: accountRemovalDlg.open();
        }
    }

    Kirigami.FormLayout {
        Layout.fillWidth: true
        Layout.margins: Kirigami.Units.largeSpacing
        Controls.Label {
            visible: servicesList.count === 0
            Layout.fillWidth: true
            height: component.height / 3
            enabled: false
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            text: i18nc("A text shown when an account has no configurable services", "There are no configurable services available for this account. You can still change its display name by clicking the edit icon above.")
        }
        Kirigami.Separator {
            visible: servicesList.count > 0
            Kirigami.FormData.label: i18nc("Heading for a list of services available with this account", "Use This Account For")
            Kirigami.FormData.isSection: true
        }
        Repeater {
            id: servicesList
            delegate: Controls.CheckBox {
                id: serviceCheck
                Kirigami.FormData.label: model.description
                checked: model.enabled
                text: model.displayName
                Binding {
                    target: serviceCheck
                    property: "checked"
                    value: model.enabled
                }
                onClicked: {
                    var job = serviceToggleJob.createObject(component, { "accountId": servicesList.model.accountId, "serviceId": model.name, "serviceEnabled": !model.enabled })
                    job.start()
                }
            }
        }
    }
}
