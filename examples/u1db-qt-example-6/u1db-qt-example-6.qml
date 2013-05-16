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


Item {

        width: units.gu(45)
        height: units.gu(80)

        U1db.Database {
            id: aDatabase
            path: "aDatabase6"
        }

        U1db.Database {
            id: aTargetDatabase
            path: "aTargetDatabase6"
        }

       U1db.Document {
            id: aDocument1
            database: aDatabase
            docId: 'helloworld'
            create: true
            defaults:{"hello": { "world": { "message":"Hello World", "id": 1 } } }

        }

       U1db.Document {
            id: aDocument2
            database: aTargetDatabase
            docId: 'helloworld'
            create: true
            defaults:{"hello": { "world": [
                        { "message":"Hello World", "id": 2 },
                        { "message":"Hello World", "id": 2.5 }
                    ] } }
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

       U1db.Index{
           database: aTargetDatabase
           id: by_target_helloworld
           expression: ["hello.world.id","hello.world.message"]
       }

       U1db.Query{
           id: aTargetQuery
           index: by_target_helloworld
           query: [{"id":"*"},{"message":"Hel*"}]
       }

       U1db.Synchronizer{
           id: aSynchronizer
           source: aDatabase
           local_targets: [aTargetDatabase]
           remote_targets: ["http://somewhere"]
           resolve_to_source: true
           synchronize: false
       }

    MainView {

        id: u1dbView
        width: units.gu(45)
        height: units.gu(80)
        anchors.top: parent.top;

        Tabs {
            id: tabs
            anchors.fill: parent

            Tab {
                objectName: "Tab1"

                title: i18n.tr("Hello U1Db!")

                page: Page {
                    id: helloPage

                    Rectangle {
                         width: units.gu(45)
                         height: units.gu(70)
                         anchors.top: parent.top;

                        ListView {
                            width: units.gu(45)
                            height: units.gu(35)
                            anchors.top: parent.top;
                            model: aQuery

                            delegate: Text {
                                x: 66; y: 77
                                text: {
                                    text: "(" + index + ") '" + contents.message + " " + contents.id + "'"
                                }
                            }
                        }

                        ListView {
                            width: units.gu(45)
                            height: units.gu(35)
                            anchors.bottom: parent.bottom;
                            model: aTargetQuery

                            delegate: Text {
                                x: 66; y: 77
                                text: {
                                    text: "(" + index + ") '" + contents.message + " " + contents.id + "'"
                                }
                            }
                        }
                    }

                   Rectangle {
                       width: units.gu(45)
                       height: units.gu(5)
                       anchors.bottom: parent.bottom;
                       color: "white"

                       Button{
                           text: "Sync"
                           onClicked: aSynchronizer.synchronize = true
                           anchors.left: parent.left
                           anchors.right: parent.right

                       }

                   }
                }

            }

        }

    }

}


