import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Ubuntu.OnlineAccounts 0.1 as OA
import org.kde.kaccounts 1.0

ApplicationWindow
{
    StackView {
        id: stack
        anchors.fill: parent

        initialItem: ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            header: Label {
                font.pointSize: 20
                text: "Accounts"
            }
            footer:  Button {
                text: "Add new Account"
                onClicked: stack.push(addProviderComponent)
            }

            model: OA.AccountServiceModel {
                id: accountsModel
                service: "global"
                includeDisabled: true
            }

            delegate: Label {
                text: displayName
            }
        }
    }

    Component {
        id: addProviderComponent
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            header: Label {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 20
                text: "Available Accounts"
            }

            model: OA.ProviderModel {}
            delegate: Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: displayName

                Component {
                    id: jobComponent
                    CreateAccountJob {}
                }

                onClicked: {
                    var job = jobComponent.createObject(stack, { providerName: providerId})
                    job.start()
                }
            }
        }
    }
}
