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

    MainView {

        id: u1dbView
        width: units.gu(45)
        height: units.gu(80)
        anchors.top: parent.top;

        /*!

            A Database is very simple to create. It only needs an id and a path where the file will be created. A Database is a model, which can be used by elements, such as the ListView further in this example.

            U1db.Database {
                id: aDatabase
                path: "aU1DbDSatabase2"
            }

        */

        U1db.Database {
            id: aDatabase
            path: "aU1DbDSatabase3"
        }

        /*!

            A Document can be declared at runtime. It requires at the very least a unique 'docId', but that alone won't do anything special. In order for a document to be entered into the database the below snippet demonstrates the basic requirements. The id might be optional.

            U1db.Document {
            id: aDocument
            database: aDatabase
            docId: 'helloworld'
            create: true
            defaults: { "hello":"Hello World" }
            }

        */

       U1db.Document {
        id: aDocument
        database: aDatabase
        docId: 'helloworld'
        create: true
        defaults: { "hello":"Hello World" }
        }

        Tabs {
            id: tabs
            anchors.fill: parent

            Tab {
                objectName: "Tab1"

                title: i18n.tr("Hello U1Db!")

                page: Page {

                    id: helloPage

                    // Here we define a rectangle that represents the lower portion of our application. It will contain all the main parts of the application.

                    Rectangle {

                         width: units.gu(45)
                         height: units.gu(70)
                         anchors.bottom: parent.bottom

                         color: "#00FFFFFF"

                         /* The following ListView, which contains a TextArea for displaying contents from the database.

                         The ListView should probably be removed in this case since we are not displaying multiple records from the database. */

                         ListView {

                            width: units.gu(45)
                            height: units.gu(60)
                            anchors.bottom: parent.bottom

                            TextArea{

                                x: units.gu(1)
                                y: units.gu(1)
                                width: units.gu(43)
                                height: units.gu(58)

                                color: "#FFFFFF"

                            }

                         }

                         // This rectangle contains our navigation controls

                         Rectangle {

                              width: units.gu(43)
                              height: units.gu(5)
                              anchors.top: addressBarListView.bottom
                              x: units.gu(1.5)

                              color: "#00FFFFFF"

                              Row{

                                 width: units.gu(43)
                                 height: units.gu(5)
                                 anchors.verticalCenter: parent.verticalCenter
                                 spacing: units.gu(2)


                                 Button {
                                 text: "<"
                                 onClicked: print("clicked Back Button")
                                 }
                                 Button {
                                 text: "Home"
                                 onClicked: print("clicked Home Button")
                                 }
                                 Button {
                                 text: "+"
                                 onClicked: print("clicked Save Button")
                                 }
                                 Button {
                                 text: ">"
                                 onClicked: print("clicked Forward Button")
                                 }

                              }

                          }

                       ListView {

                            width: units.gu(45)
                            height: units.gu(5)
                            anchors.top: parent.top

                            id: addressBarListView

                            /*! Inside this example ListView is a reference to the Database model mentioned earlier:

                                ListView {

                                     model: aDatabase

                                 }

                            */

                            model: aDatabase

                           /*!

                               Once a model is assigned to a ListView a delegate can represent each Document retrieved from the Database.

                                ListView {

                                     model: aDatabase

                                     delegate: Rectangle{
                                         anchors.right: parent.right
                                     }

                                 }


                           */

                            delegate: TextField {

                                    id: addressBar

                                    width: units.gu(43)
                                    anchors.verticalCenter: parent.verticalCenter
                                    x: units.gu(1)

                                    onAccepted: {

                                        onClicked: updateContent(contents.hello,addressBar.text)

                                    }

                                    function updateContent(documentText, addressBarText) {
                                        if(documentText!==addressBarText){
                                            print(documentText+" * "+addressBarText)
                                        }
                                    }

                                    text: {
                                        /*!
                                            The object called 'contents' contains a string as demonstrated here. In this example 'hello' is our search string.


                                        text: contents.hello



                                        */

                                        text: contents.hello

                                    }

                                }


                        }

                    }


                }

            }

        }

    }

}
