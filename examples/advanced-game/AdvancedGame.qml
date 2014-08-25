/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *  Nekhelesh Ramananthan <nik90@ubuntu.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2
import Ubuntu.Components 1.1

// Import U1db to access its functions
import U1db 1.0 as U1db

MainView {
    id: mainView

    objectName: "mainView"
    applicationName: "com.ubuntu.developer.nik90.AdvancedGame"

    width: units.gu(50)
    height: units.gu(75)

    useDeprecatedToolbar: false

    // U1db database to store player profiles
    U1db.Database {
        id: appDb
        path: "playerDatabase"
    }

    // Common Action available to all pages
    Action {
        id: addPlayerAction
        text: i18n.tr("Add Player")
        iconName: "add"
        onTriggered: pagestack.push(Qt.resolvedUrl("CreatePlayerPage.qml"))
    }

    PageStack {
        id: pagestack

        Component.onCompleted: push(homePage)

        Page {
            id: homePage
            title: i18n.tr("Advanced Game")

            head.actions: [
                addPlayerAction
            ]

            Column {
                anchors.fill: parent
                anchors.margins: units.gu(5)
                spacing: units.gu(2)

                Button {
                    width: parent.width
                    text: i18n.tr("List Players")
                    onClicked: pagestack.push(Qt.resolvedUrl("ListPlayers.qml"))
                }

                Button {
                    width: parent.width
                    text: i18n.tr("Filter Players")
                    onClicked: pagestack.push(Qt.resolvedUrl("FilterPlayers.qml"))
                }
            }
        }
    }
}
