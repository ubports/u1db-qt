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
import Ubuntu.Components.ListItems 0.1 as ListItem
import U1db 1.0 as U1db

Page {
    id: homePage
    title: i18n.tr("List Players")

    U1db.Index {
        database: appDb
        id: by_player
        expression: ["username", "userlevel", "userclass"]
    }

    U1db.Query {
        id: playerQuery
        index: by_player
        query: ["*", "*", "*"]
    }
    
    Label {
        anchors.centerIn: parent
        visible: players.count === 0
        text: "No players? Add some!"
    }
    
    ListView {
        id: players
        anchors.fill: parent
        clip: true
        model: playerQuery
        
        delegate: ListItem.Subtitled {
            text: model.contents.username
            subText: "User Level: " + model.contents.userlevel
            removable: true
            confirmRemoval: true
            onItemRemoved: appDb.deleteDoc(model.docId)
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
