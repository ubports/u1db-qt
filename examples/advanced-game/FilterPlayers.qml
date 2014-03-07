import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
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
                subText: userClass.model[contents.userclass]
                removable: true
                confirmRemoval: true
                onItemRemoved: appDb.deleteDoc(model.docId)
            }
        }
    }
}
