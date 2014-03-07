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

import QtQuick 2.0
import Ubuntu.Components 0.1
import U1db 1.0 as U1db

Page {
    id: createUserPage

    visible: false
    title: i18n.tr("Create Player")

    actions: [
        Action {
            id: savePlayerAction
            text: i18n.tr("Save Player")
            iconSource: "image://theme/save"
            onTriggered: {
                appDb.putDoc({ "username": userName.text, "userlevel": userlevel.text, "userclass": userClass.selectedIndex})
                pageStack.pop()
            }
        }
    ]

    Column {
        spacing: units.gu(3)
        anchors.fill: parent
        anchors.margins: units.gu(2)

        Column {
            width: parent.width
            spacing: units.gu(1)
            Label { text: "Username" }
            TextField {
                id: userName
                placeholderText: "Username"
                width: parent.width
            }
        }

        Column {
            width: parent.width
            spacing: units.gu(1)
            Label { text: "User Level" }
            TextField {
                id: userlevel
                placeholderText: "User Level"
                width: parent.width
            }
        }

        OptionSelector {
            id: userClass
            model: ["Foot Soldier", "Archer", "Giant", "Wizard", "Demolisher"]
        }
    }

    tools: ToolbarItems {
        id: toolbarSettings

        ToolbarButton {
            id: addPlayer
            action: savePlayerAction
        }
    }
}
