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

/*!

This example and tutorial is designed to show a wide variety of U1Db-Qt functionality and usage. The example demonstrates:

\list 1
    \li Using Index and Query elements
\endlist

  */

Item {

        width: units.gu(45)
        height: units.gu(80)

        /*!

            A Database is very simple to create. It only needs an id and a path where the file will be created. A Database is a model, which can be used by elements, such as the ListView further in this example.

            U1db.Database {
                id: aDatabase
                path: "aDatabase4"
            }

        */

        U1db.Database {
            id: aDatabase
            path: "aDatabase5"
        }

        /*!

            A Document can be declared at runtime. It requires at the very least a unique 'docId', but that alone won't do anything special. The snipet below snippet demonstrates the basic requirements.

            In addition to this, this example displays text from the database for a specific docId and id key in a text area called 'documentContent. To update the text area at startup with either the default value or a value from the database the onCompleted function is utilized, which is also demonstrated below.

            U1db.Document {
                id: aDocument
                database: aDatabase
                docId: 'helloworld'
                create: true
                defaults: { "helloworld":"Hello World" }

                Component.onCompleted: {
                    documentContent.text = aDocument.contents.helloworld
                }

            }

        */


       U1db.Document {
            id: aDocument
            database: aDatabase
            docId: 'helloworld'
            create: true
            defaults:{"hello": { "world": [{ "message":"Hello World", "id": 1 }] } }

        }
       U1db.Document {
            id: aDocument2
            database: aDatabase
            docId: 'helloworld2'
            create: true
            defaults:{"hello": { "world": [{ "message":"Hello World", "id": 2 }] } }

        }

       U1db.Index{
           database: aDatabase
           id: by_helloworld
           name: "by-helloworld"
           expression: ["hello.world.id","hello.world.message"]
       }
       U1db.Query{
           id: helloListModel
           index: by_helloworld
           //query: "*"
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
                   ListView {
                        width: units.gu(45)
                        height: units.gu(80)

                        /*
                        Here is the reference to the Database model mentioned earlier.
                        */
                        model: helloListModel

                       /* A delegate will be created for each Document retrieved from the Database */
                        delegate: Text {
                            x: 66; y: 77
                            text: {
                                /*!
                                    The object called 'contents' contains a string as demonstrated here. In this example 'hello' is our search string.

                                    text: contents.hello
                                */

                                text: contents.id + " " + contents.message
                            }
                        }
                    }
                }


                }

            }




        }







    }


