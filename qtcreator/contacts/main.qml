/*
 * Copyright (C) 2013 Canonical, Ltd.
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

MainView {
    width: units.gu(45)
    height: units.gu(80)

    U1db.Database {
        id: contactsDatabase
        /*
            Uncomment to persistently store contacts on disk
        path: "contacts.db"
        */
    }

    U1db.Document {
        database: contactsDatabase
        docId: 'person0'
        create: true
        defaults: { 'name': 'John', 'city': 'Dublin', 'phone': 65849 }
    }

    U1db.Document {
        database: contactsDatabase
        docId: 'person1'
        create: true
        defaults: { 'name': 'Ivanka', 'city': 'Dublin', 'phone': 98765 }
    }

    U1db.Document {
        database: contactsDatabase
        docId: 'person2'
        create: true
        defaults: { 'name': 'Leonardo', 'city': 'Rome', 'phone': 12345 }
    }

    U1db.Index {
        database: contactsDatabase
        id: byCityName
        expression: [ "city" ]
    }

    U1db.Query {
        id: numberOne
        index: byCityName
        query: [ "Dublin" ]
    }

    Page {
        Tabs {
            Tab {
                title: i18n.tr("People living in Dublin")
                page: Page {
                    anchors.centerIn: parent
                    ListView {
                        width: units.gu(45)
                        height: units.gu(80)
                        model: numberOne
                        delegate: ListItem.Subtitled {
                            text: contents.name
                            subText: contents.city
                        }
                    }
                }
            }
         }
    }
}

