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

                    /*! Here we define a rectangle that represents the lower portion of our application. It will contain all the main parts of the application.

                    Rectangle {

                         width: units.gu(45)
                         height: units.gu(70)
                         anchors.bottom: parent.bottom

                         color: "#00FFFFFF"

                         // The remainder of the main part of the application goes here ...

                         }

                     */

                    Rectangle {

                         width: units.gu(45)
                         height: units.gu(70)
                         anchors.bottom: parent.bottom

                         color: "#00FFFFFF"



                         Rectangle {

                            width: units.gu(45)
                            height: units.gu(60)
                            anchors.bottom: parent.bottom

                            /*

                            The following TextArea is for displaying contents from the database for the current state of the global document, as defined by the key / name in the address bar.

                            TextArea{

                                id: documentContent

                                selectByMouse : false

                                x: units.gu(1)
                                y: units.gu(1)
                                width: units.gu(43)
                                height: units.gu(58)
                                color: "#000000"

                            }

                             */

                            TextArea{

                                id: documentContent

                                selectByMouse : false

                                x: units.gu(1)
                                y: units.gu(1)
                                width: units.gu(43)
                                height: units.gu(58)
                                color: "#000000"

                            }

                         }

                         // This rectangle contains our navigation controls

                         Rectangle {

                              width: units.gu(43)
                              height: units.gu(5)
                              anchors.top: addressBarArea.bottom
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

                       Rectangle {

                           id: addressBarArea

                            width: units.gu(45)
                            height: units.gu(5)
                            anchors.top: parent.top

                            TextField {

                                    id: addressBar

                                    width: units.gu(43)
                                    anchors.verticalCenter: parent.verticalCenter
                                    x: units.gu(1)

                                    /*!
                                        There is an object within in the 'aDocument' model defined earlier called 'contents', which contains a key called 'hello', which represents a search string.  In our example the key will represent the name of a document in the database, which will be displayed in the address bar. Displaying the key is demonstrated here:

                                    text: displayKey(aDocument.contents)

                                    function displayKey(documentObject){

                                        var keys = Object.keys(documentObject);

                                        return keys[0]

                                    }

                                    */

                                    text: getCurrentDocumentKey(aDocument.contents)

                                    function getCurrentDocumentKey(documentObject){

                                        if(typeof documentObject!='undefined'){

                                            var keys = Object.keys(documentObject);

                                            print("keys[0] = " + keys[0])

                                            return keys[0]

                                        }

                                        else{

                                            return ''
                                        }



                                    }

                                    onAccepted: {

                                        onClicked: updateContent(getCurrentDocumentKey(aDocument.contents),addressBar.text)

                                    }


                                    function updateContent(documentText, addressBarText) {

                                        if(documentText!==addressBarText){

                                            /*!

                                              Here we create a new object that will become the contents of the global document, including some default text.

                                            documentContent.text = 'More Hello World...';

                                            var newContents = {};
                                            var newFieldName = addressBarText;
                                            aDocument.docId = addressBarText;
                                            newContents[newFieldName]= documentContent.text;
                                            aDocument.contents=newContents

                                              */

                                            documentContent.text = 'More Hello World...';

                                            var newContents = {};
                                            var newFieldName = addressBarText;
                                            aDocument.docId = addressBarText;
                                            newContents[newFieldName]= documentContent.text;
                                            aDocument.contents=newContents

                                        }

                                    }


                                }


                        }

                    }


                }

            }

        }

    }

}
