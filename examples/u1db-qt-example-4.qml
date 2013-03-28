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
                path: "aDatabase4"
            }

        */

        U1db.Database {
            id: aDatabase
            path: "aDatabase4"
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

       /*!

         It should be possible to use a document without a database, as demonstrated in this snippet. Additionally this document will use the concept of sub-keys, as exemplified by the "bookmarks" id key + contents. This example will attempt to use the bookmark document to store docId values from the database, which will be displayed in a ListView on the second tab of the application. The user will be able to select a value from the ListView and the first tab will be modified accordingly.

       U1db.Document {
            id: aBookmarkDocument
            docId: 'bookmarks'
            create: true
            defaults: { "bookmarks": [{}] }
       }


         */


       U1db.Document {
            id: aBookmarkDocument
            docId: 'bookmarks'
            create: true
            defaults: { "bookmarks": [{}] }
       }


       function switchToPreviousDocument(documentObject){

          aDocument.docId = getPreviousDocumentId(documentObject)

          }

       function switchToNextDocument(){

          aDocument.docId = getNextDocumentId(aDocument)

        }

       function getPreviousDocumentId(documentObject){

           if(typeof documentObject!='undefined'){

               /*!

                 The listDocs method retrieves all the docId values from the current database. In this demonstration the values are put into an array, which is then checked to locate the docId for the current and previous documents within the database.

               var documentIds = {}

               documentIds = documentObject.database.listDocs()

               for(var i = 0; i < documentIds.length; i++){

                   if(documentIds[i]===documentObject.docId && i > 0){
                       return documentIds[i-1]
                   }
                   else if(documentIds[i]===documentObject.docId && i==0){
                       return documentIds[documentIds.length-1]
                   }

               }

                 */

               var documentIds = {}

               documentIds = documentObject.database.listDocs()

               for(var i = 0; i < documentIds.length; i++){

                   if(documentIds[i]===documentObject.docId && i > 0){
                       return documentIds[i-1]
                   }
                   else if(documentIds[i]===documentObject.docId && i==0){
                       return documentIds[documentIds.length-1]
                   }

               }

               return documentIds[0]

           }

           else{

               print("Error!")

               return ''
           }

       }

       function getNextDocumentId(documentObject){

           if(typeof documentObject!='undefined'){

               var documentIds = documentObject.database.listDocs()

               for(var i = 0; i < documentIds.length; i++){

                   if(documentIds[i]===documentObject.docId && i < (documentIds.length-1)){
                       return documentIds[i+1]
                   }
                   else if(documentIds[i]===documentObject.docId && i==(documentIds.length-1)){
                       return documentIds[0]
                   }

               }

               return documentIds[0]

           }

           else{

               print("Error!")

               return ''
           }

       }

        function getCurrentDocumentKey(contentsObject){

            if(typeof contentsObject!='undefined'){

                var keys = Object.keys(contentsObject);

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

                These steps demonstrate the creation of a temporary document, based on a copy of the global document. This will then be used to determine if there is already a document in the database with the same docId as the address bar, and additionally with a key id with the same name.

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

                            /*!

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

                            TextArea {

                                id: documentContent

                                selectByMouse : false

                                x: units.gu(1)
                                y: units.gu(1)
                                width: units.gu(43)
                                height: units.gu(58)
                                color: "#000000"

                            }

                         }

                         // This rectangle contains the navigation controls

                         Rectangle {

                              width: units.gu(43)
                              height: units.gu(5)
                              anchors.top: addressBarArea.bottom
                              x: units.gu(1.5)
                              color: "#00FFFFFF"

                              Row {

                                 width: units.gu(43)
                                 height: units.gu(5)
                                 anchors.verticalCenter: parent.verticalCenter
                                 spacing: units.gu(2)

                                 Button {
                                 text: "<"
                                 onClicked: updateContentWindow(switchToPreviousDocument(aDocument), addressBar.text)
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
                                 onClicked: updateContentWindow(switchToNextDocument(aDocument), addressBar.text)
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

                                    hasClearButton: false

                                    /*!

                                        There is an object within in the 'aDocument' model defined earlier called 'contents', which contains a key called 'hello', which represents a search string.  In this example the key will represent the name of a document in the database, which will be displayed in the address bar. Displaying the key is demonstrated here:

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

            Tab {
                objectName: "Tab2"

                title: i18n.tr("Bookmarks")

                page: Page {

                    id: bookmarkPage

                }

            }


        }







    }

}
