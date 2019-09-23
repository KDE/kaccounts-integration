/*
 *   Copyright 2015 Martin Klapetek <mklapetek@kde.org>
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

import org.kde.kaccounts 1.0
import org.kde.kcm 1.2

import Ubuntu.OnlineAccounts 0.1 as OA

ScrollViewKCM {
    id: kaccountsRoot
    objectName: "kaccountsModule"

    // Existing accounts
    view: ListView {
        clip: true
        model: OA.AccountServiceModel {
            id: accountsModel
            service: "global"
            includeDisabled: true
        }

        delegate: Kirigami.SwipeListItem {
            width: ListView.view.width

            contentItem: Controls.Label {
                text: i18n("%1 (%2)", model.displayName, providerName)

                OA.Account {
                    id: account
                    objectHandle: model.accountHandle
                }
            }
            actions: [
                Kirigami.Action {
                    iconName: "bookmark-remove"
                    onTriggered: {
                        account.remove()
                    }
                }
            ]
            onClicked: kcm.push("AvailableServices.qml", {accountId: model.accountId})
        }
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
