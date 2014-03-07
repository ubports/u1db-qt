import QtQuick 2.0
import Ubuntu.Components 0.1
// Import U1db to access its functions
import U1db 1.0 as U1db

MainView {
    objectName: "mainView"
    applicationName: "com.ubuntu.developer.nik90.AdvancedGame"

    width: units.gu(50)
    height: units.gu(75)
    backgroundColor: UbuntuColors.coolGrey

    // U1db database to store player profiles
    U1db.Database {
        id: appDb
        path: "playerDatabase"
    }

    // Common Toolbar action available to all pages
    actions: [
        Action {
            id: addPlayerAction
            text: i18n.tr("Add Player")
            iconSource: "image://theme/add"
            onTriggered: pagestack.push(Qt.resolvedUrl("CreatePlayerPage.qml"))
        }
    ]

    PageStack {
        id: pagestack

        Component.onCompleted: push(homePage)

        Page {
            id: homePage
            title: "Advanced Game :P"

            Column {
                anchors.fill: parent
                anchors.margins: units.gu(5)
                spacing: units.gu(2)

                Button {
                    width: parent.width
                    text: "List Players"
                    onClicked: pagestack.push(Qt.resolvedUrl("ListPlayers.qml"))
                }

                Button {
                    width: parent.width
                    text: "Filter Players"
                    onClicked: pagestack.push(Qt.resolvedUrl("FilterPlayers.qml"))
                }
            }

            tools: ToolbarItems {
                id: toolbarSettings

                ToolbarButton {
                    id: addPlayer
                    action: addPlayerAction
                }
            }
        }
    }
}
