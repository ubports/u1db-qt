/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *  Christian Dywan <christian.dywan@canonical.com>
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
import U1db 1.0 as U1db
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1

/*!

This example and tutorial is designed to show a wide variety of U1Db-Qt functionality and usage. The example demonstrates:

\list 1
    \li Combining U1Db-Qt with elements and components that do not utilize models
    \li Blending the U1Db-Qt plugin with QML and Javascript
\endlist

  */

MainView {
    id: root
    applicationName: "com.ubuntu.developer.foobar.bookmarks"

    width: units.gu(45)
    height: units.gu(80)

    /*
        Bookmarks database
    */

    U1db.Database {
        id: db
        // path: "bookmarks.db"
    }

    U1db.Document {
        database: db
        docId: 'defaultsDuckDuckGo'
        create: true
        defaults: { "uri": "https://www.duckduckgo.com", visited: false, "meta": { "title": "Search DuckDuckGo", visits: 0, tags: [ 'search', 'engine' ] } }
    }

    U1db.Document {
        database: db
        docId: 'defaultsUbuntu'
        create: true
        defaults: { "uri": "http://www.ubuntu.com", visited: true, "meta": { "title": "The world's most popular free OS", visits: 1001 } }
    }

    U1db.Query {
        id: allBookmarks
        index: U1db.Index {
            database: db
        }
    }

    /*
        UI: details
    */
    Component {
        id: detailsPopCom
        Popover {
            id: detailsPop
            Column {
                anchors.centerIn: parent
                spacing: units.gu(1)
                Label {
                    text: i18n.tr('JSON')
                }
                TextField {
                    text: bookmarksList.detailsDocId
                }
                TextArea {
                    text: bookmarksList.detailsContents.replace(',',',\n')+'\n\n'
                    readOnly: true
                    autoSize: true
                }
                Button {
                    text: i18n.tr('Delete')
                    onClicked: {
                        PopupUtils.close(detailsPop)
                        db.deleteDoc(bookmarksList.detailsDocId)
                    }
                }
            }
        }
    }

    /*
        UI: list view, filters
    */

    Page {
        id: page
        title: i18n.tr("Bookmarks")

        Item {
            id: container
            anchors.margins: units.gu(1)
            anchors.fill: parent
 
            ListView {
                id: bookmarksList
                anchors.fill: parent
                model: allBookmarks
                property string detailsDocId: ""
                property string detailsContents: ""
                delegate: ListItem.Subtitled {
                    text: contents.title || '[title:%1]'.arg(docId)
                    subText: contents.uri || '[uri:%1]'.arg(docId)
                    // iconSource: contents.uri + "/favicon.ico"
                    fallbackIconName: "favorite-unselected,text-html"
                    iconFrame: false
                    onClicked: {
                        bookmarksList.detailsDocId = docId
                        bookmarksList.detailsContents = JSON.stringify(contents)
                        PopupUtils.open(detailsPopCom, bookmarksList)
                    }
                }
            }

            OptionSelector {
                id: filterSelector
                StateSaver.properties: 'selectedIndex'
                anchors.bottom: parent.bottom
                text: i18n.tr('N/A')
                expanded: true
                model: ListModel {
                    ListElement { label: 'Newly Added'; expression: "[ 'meta.visits' ]"; query: "[ { 'visits': 0 } ]" }
                    ListElement { label: 'Ubuntu'; expression: "[ 'uri' ]"; query: "[ 'http://www.ubuntu*' ]" }
                    ListElement { label: 'Search'; expression: "[ 'meta.title' ]"; query: "[ 'Search*' ]" }
                    ListElement { label: 'Engine'; expression: "[ 'meta.tags' ]"; query: "[ 'engine' ]" }
                    ListElement { label: 'All'; expression: "[ 'meta.visits', 'meta.title' ]"; query: "[ '*', '*' ]" }
                }
                delegate: OptionSelectorDelegate {
                    text: i18n.tr(label)
                }
                selectedIndex: model.count - 1
                onSelectedIndexChanged: {
                    var d = model.get(selectedIndex)
                    text = '%1 - %2'.arg(d.expression).arg(d.query)
                    allBookmarks.index.expression = eval(d.expression)
                    allBookmarks.query = eval(d.query)
                }
            }
        }
    }
}
