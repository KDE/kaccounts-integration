/*
 *   Copyright 2015 (C) Martin Klapetek <mklapetek@kde.org>
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

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

ApplicationWindow {
    id: kaccountsRoot
    objectName: "_root"
//     id: _root

    width: 500; height: 800


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: units.largeSpacing

        PlasmaExtras.Title {
            text: i18n("Add new ownCloud account")
        }

        StackView {
            id: stack

            Layout.fillHeight: true
            Layout.fillWidth: true

            initialItem: BasicInfo {
                id: basicInfoPage
                objectName: "basicInfoPage"
            }
        }

        Component {
            id: servicesComponent

            Services {
                objectName: "servicesPage"
            }
        }

        RowLayout {
            PlasmaComponents.Button {
                id: backButton
                Layout.fillWidth: true
                text: i18n("Back");
                enabled: stack.currentItem.objectName == "servicesPage"

                onClicked: {
                    if (stack.currentItem.objectName == "servicesPage") {
                        stack.pop(servicesComponent);
                    }
                }
            }

            PlasmaComponents.Button {
                id: nextButton
                Layout.fillWidth: true
                text: i18n("Next")
                enabled: basicInfoPage.canContinue //: false
                visible: stack.currentItem == basicInfoPage

                onClicked: {
                    if (stack.currentItem == basicInfoPage) {
                        stack.push(servicesComponent);
                    }
                }
            }

            PlasmaComponents.Button {
                id: finishButton
                Layout.fillWidth: true
                text: i18n("Finish")
                visible: stack.currentItem.objectName == "servicesPage"

                onClicked: {
                    helper.finish(stack.currentItem.contactsEnabled ? "" : "contacts");
                }
            }
        }
    }
}
