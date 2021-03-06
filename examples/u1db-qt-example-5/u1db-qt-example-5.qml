/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Kevin Wright <kevin.wright@canonical.com>
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

MainView {
    width: units.gu(45)
    height: units.gu(80)

        U1db.Database {
            id: aDatabase
            path: Qt.resolvedUrl("aDatabase5")
        }

       U1db.Document {
            database: aDatabase
            docId: 'helloworld1'
            create: true            
            defaults:{"hello": { "world": { "message":"Hello World", "id": 1 } } }

        }

       U1db.Document {
            database: aDatabase
            docId: 'helloworld2'
            create: true
            defaults:{"hello": { "world": [
                        { "message":"Hello World", "id": 2 },
                        { "message":"Hello World", "id": 2.5 }
                    ] } }
        }

       U1db.Document {
            database: aDatabase
            docId: 'helloworld3'
            contents:{"hello": { "world": [
                        { "message":"Hello World", "id": 3 },
                        { "message":"Hello World", "id": 3.33 },
                        { "message":"Hello World", "id": 3.66 }
                    ] } }
        }

       U1db.Document {
            database: aDatabase
            docId: 'helloworld4'
            defaults:{"hello": { "world": { "message":"Hello World", "id": 4 } } }
        }

       U1db.Index{
           database: aDatabase
           id: by_helloworld
           expression: ["hello.world.id","hello.world.message"]
       }

       U1db.Query{
           id: aQuery
           index: by_helloworld
           query: [{"id":"*"},{"message":"Hel*"}]
       }

        Tabs {
            id: tabs

            Tab {
                title: i18n.tr("Hello U1Db!")

                page: Page {
                    id: helloPage

                    ListView {
                        width: units.gu(45)
                        height: units.gu(80)
                        model: aQuery
                        delegate: Text {
                            text: "(" + index + ") '" + contents.message + " " + contents.id + "'"
                    }
                }
            }
        }
    }
}

