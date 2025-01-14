/*
 *   SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
 *   SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigami.delegates as KD
import org.kde.kcmutils as KCM

import org.kde.kaccounts as KAccounts

KCM.ScrollViewKCM {
    id: root
    title: i18nd("kaccounts-integration", "Add New Account")

    headerPaddingEnabled: false // Let the InlineMessage touch the edges
    header: Kirigami.InlineMessage {
        id: errorMessage
        position: Kirigami.InlineMessage.Position.Header
        type: Kirigami.MessageType.Error
        showCloseButton: true
        visible: false
    }

    view: ListView {

        id: accountListView
        clip: true

        model: KAccounts.ProvidersModel {}

        delegate: Controls.ItemDelegate {
            id: account

            width: ListView.view.width

            icon.name: model.iconName
            text: model.displayName
            enabled: model.supportsMultipleAccounts === true || model.accountsCount === 0

            contentItem: KD.IconTitleSubtitle {
                icon: icon.fromControlsIcon(account.icon)
                title: account.text
                subtitle: model.description
                selected: account.highlighted || account.down
                font: account.font
                wrapMode: Text.Wrap
            }

            onClicked: {
                var job = jobComponent.createObject(root, { "providerName": model.name })
                job.start()
                accountListView.currentIndex = -1
            }
        }
    }

    Component {
        id: jobComponent
        KAccounts.CreateAccountJob {
            onFinished: {
                // Don't close when there is an error to show an error message
                if (error == 0) {
                    kcm.pop()
                } else {

                    if (error === 1) { // KJob::KilledJobError, cancelled by user
                        return
                    }

                    errorMessage.text = errorText
                    errorMessage.visible = true
                }
            }
        }
    }
}
