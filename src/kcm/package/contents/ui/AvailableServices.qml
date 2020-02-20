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
import org.kde.kaccounts 1.1 as KAccounts
import org.kde.kcm 1.2
import Ubuntu.OnlineAccounts 0.1 as OA

ScrollViewKCM {
    id: component;

    title: i18n("Available Services")

    property alias accountId: servicesModel.accountId

    view: ListView {
        model: OA.AccountServiceModel {
            id: servicesModel
            includeDisabled: true
            function refreshData() {
                // Because AccountServiceModel seems to not pick this up itself, we'll reset the model... like so, because there's no reset
                var oldId = component.accountId;
                component.accountId = "";
                component.accountId = oldId;
            }
        }
        delegate: Kirigami.AbstractListItem {
            width: parent.width
            Controls.CheckBox {
                id: serviceCheck
                text: model.serviceName
                checked: model.enabled
                Binding {
                    target: serviceCheck
                    property: "checked"
                    value: model.enabled
                }
                onClicked: {
                    var job = jobComponent.createObject(component, { "accountId": component.accountId, "serviceId": model.serviceName, "serviceEnabled": !model.enabled })
                    job.result.connect(servicesModel.refreshData);
                    job.start()
                }
            }
        }
        Controls.Label {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            visible: parent.count === 0
            opacity: 0.5
            text: i18nc("A text shown when an account has no configurable services", "(No services for this account)")
        }
    }
    Component {
        id: jobComponent
        KAccounts.AccountServiceToggle { }
    }
}
