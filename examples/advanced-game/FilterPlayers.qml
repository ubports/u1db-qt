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
import Ubuntu.Components.ListItems 1.0 as ListItem
import U1db 1.0 as U1db

Page {
    id: filterPlayers

    visible: false
    title: "Filter Players"

    U1db.Index {
        database: appDb
        id: by_player
        expression: ["username", "userlevel", "userclass"]
    }

    U1db.Query {
        id: playerQuery
        index: by_player
        query:  [{username:'*'}, {userlevel:'*'},{userclass:userClass.selectedIndex.toString()}]
    }

    Column {
        spacing: units.gu(3)
        anchors.fill: parent
        anchors.margins: units.gu(2)

        Label { text: "Filter by user class" }

        OptionSelector {
            id: userClass
            model: ["Foot Soldier", "Archer", "Giant", "Wizard", "Demolisher"]
        }

        ListView {
            id: players

            width: parent.width
            height: units.gu(20)

            clip: true
            model: playerQuery

            delegate: ListItem.Subtitled {
                text:  '%1 Lvl %2'.arg(model.contents.username).arg(model.contents.userlevel)
                subText: userClass.model[model.contents.userclass]
                removable: true
                confirmRemoval: true
                onItemRemoved: appDb.deleteDoc(model.docId)
            }
        }
    }
}
