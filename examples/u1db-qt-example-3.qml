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
    \li Combining U1Db-Qt with elements and components that do not utilize models
    \li Blending the U1Db-Qt plugin with QML and Javascript
\endlist

  */

Item {

        width: units.gu(45)
        height: units.gu(80)

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
            defaults: { "helloworld":"Hello World" }

            Component.onCompleted: {
                documentContent.text = aDocument.contents.helloworld
            }

        }

        function getCurrentDocumentKey(documentObject){

            if(typeof documentObject!='undefined'){

                var keys = Object.keys(documentObject);

                return keys[0]

            }

            else{

                return ''
            }



        }


        function updateContentWindow(documentText, addressBarText) {

            // Somewhere below need to check for things like invalid docId

            if(documentText!==addressBarText) {

                /*!

                Thes steps demonstrate the creation of a temporary document, based on a copy of the global document. This will then be used to determine if there is already a document in the database with the same docId as the address bar, and additionally with a key id with the same name.

                var tempDocument = {}
                var tempFieldName = addressBarText;
                var tempContents = {};

                tempDocument = aDocument
                tempDocument.docId = addressBarText;

                tempContents = tempDocument.contents

                NOTE: For simplicity sake this example sometimes uses the same value for both the docId and the key id, as seen here. Real life implimentations can and will differ, and this will be demonstrated elsewhere in the example code.

                */

                var tempDocument = {}
                var tempFieldName = addressBarText;
                var tempContents = {};

                tempDocument = aDocument
                tempDocument.docId = addressBarText;

                tempContents = tempDocument.contents

                if(typeof tempContents !='undefined' && typeof tempContents[tempFieldName]!='undefined') {

                    aDocument = tempDocument
                    documentContent.text = tempContents[tempFieldName]

                }
                else {

                    /*!

                    Here the contents of the temporary document are modified, which then replaces the global document.

                    documentContent.text = 'More Hello World...';

                    tempContents = {}
                    tempContents[tempFieldName] = documentContent.text
                    tempDocument.contents = tempContents
                    aDocument = tempDocument

                    */

                    documentContent.text = 'More Hello World...';

                    tempContents = {}
                    tempContents[tempFieldName] = documentContent.text
                    tempDocument.contents = tempContents
                    aDocument = tempDocument

                }

            }
            else {

                /*!

                In this instance the current document's content is updated from the text view. The unique key and docId are not modified because the database already contains a record with those properties.

                tempContents = {}
                tempFieldName = getCurrentDocumentKey(aDocument.contents)
                tempContents[tempFieldName] = documentContent.text
                aDocument.contents = tempContents

                */

                tempContents = {}
                tempFieldName = getCurrentDocumentKey(aDocument.contents)
                tempContents[tempFieldName] = documentContent.text
                aDocument.contents = tempContents

            }

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

                    /*! Here a rectangle is defined that represents the lower portion of our application. It will contain all the main parts of the application.

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

                            The following TextArea is for displaying contents for the current state of the global document, as defined by the key / name in the address bar.

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
                                 // add and call a function to find the document (in the database) previous to the current one, and then change both the address bar and content window to match as appropriate
                                 }
                                 Button {
                                 text: "Home"
                                 onClicked: updateContentWindow(getCurrentDocumentKey(aDocument.contents),'helloworld')
                                                                 }
                                 Button {
                                 text: "Save"
                                 onClicked: updateContentWindow(getCurrentDocumentKey(aDocument.contents),addressBar.text)
                                 }
                                 Button {
                                 text: ">"
                                 onClicked: print("clicked Forward Button")
                                 // add and call a function to find the document (in the database) next to the current one, and then change both the address bar and content window to match as appropriate
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
                                        There is an object within in the 'aDocument' model defined earlier called 'contents', which contains a key called 'helloworld', which represents a search string.  In our example the key will represent the name of a document in the database, which will be displayed in the address bar. Displaying the key is demonstrated here:

                                    text: displayKey(aDocument.contents)

                                    function displayKey(documentObject){

                                        var keys = Object.keys(documentObject);

                                        return keys[0]

                                    }

                                    */

                                    text: getCurrentDocumentKey(aDocument.contents)


                                    onAccepted: {

                                        onClicked: updateContentWindow(getCurrentDocumentKey(aDocument.contents),addressBar.text)

                                    }


                                }


                        }


                    }


                }

            }

        }

    }

}
