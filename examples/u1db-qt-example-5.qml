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

\page u1db-qt-index-tutorial.html

\title U1Db-Qt Index Tutorial

*/

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

            \code
            U1db.Database {
                id: aDatabase
                path: "aDatabase4"
            }
            \endcode

        */

        U1db.Database {
            id: aDatabase
            path: "aDatabase5"
        }

        /*!

            A Document can be instantiated at runtime, or generated dynamically. The examples below demonstrate the former.

            A very basic Document could include its unique 'id' and 'docId' properties. While it is not mandatory to define these properties, in some cases they can be convenient references. More advanced applications would likely find these very useful, and in some cases may be an absolute necessity to achieve the objectives of the program.

            This example of a very simple Document will not initially do anything, until more properties are added and defined:

            \code
            U1db.Document {
                id: aDocument1
                docId: 'helloworld1'
            }
            \endcode

        */

        /*!

           This is a very basic but still practical Document definition that contains several essential properties. In addition to 'id' and 'docId' (discussed above), the 'database', 'create', and 'defaults' properties are introduced.


        The 'database' property ensures that the Document is attached to am already defined (or possibly soon to be defined one) identified by its id (in this case 'aDatabase'). For example:

        \code
       U1db.Document {
            id: aDocument1
            database: aDatabase
            docId: 'helloworld1'
        }
        \endcode

        Should the Database not already contain a Document with the same docId ('hellowworld1' in this example) when a 'create' property is present and set to true it will be generated. For example:

        \code
       U1db.Document {
            id: aDocument1
            database: aDatabase
            docId: 'helloworld1'
            create: true
        }
        \endcode

        However, the Document still requires some data to be useful, which is what the 'defaults' property provides. The value of 'defaults' is a map of data that will be stored in the database (again when the create property is et to true). It contain key:value pairs, where the value can be a string, number, or nested object (e.g. additional fields, lists). For example:

        \code
       U1db.Document {
            id: aDocument1
            database: aDatabase
            docId: 'helloworld1'
            create: true
            defaults:{"hello": { "world": { "message":"Hello World", "id": 1 } } }

        }
        \endcode

          */

       U1db.Document {
            id: aDocument1
            database: aDatabase
            docId: 'helloworld1'
            create: true            
            defaults:{"hello": { "world": { "message":"Hello World", "id": 1 } } }

        }


       /*!

         As mentioned above, lists can also be nested in the data. Lists provide a convenient method for producing multiple instance of the same key (AKA 'field' or 'sub-field'). The example code below shows the valid use of the 'message' key (or field) multiple times within the same list.

       \code
       U1db.Document {
            id: aDocument2
            database: aDatabase
            docId: 'helloworld2'
            create: true
            defaults:{"hello": { "world": [{ "message":"Hello World", "id": 2 },{ "message":"Hello World", "id": 2.5 }] } }

        }
        \endcode



         */


       U1db.Document {
            id: aDocument2
            database: aDatabase
            docId: 'helloworld2'
            create: true
            defaults:{"hello": { "world": [{ "message":"Hello World", "id": 2 },{ "message":"Hello World", "id": 2.5 }] } }

        }

       /*!



         */

       U1db.Document {
            id: aDocument3
            database: aDatabase
            docId: 'helloworld3'
            create: true
            revise: true
            defaults:{"hello": { "world": [{ "message":"Hello World", "id": 3 },{ "message":"Hello World", "id": 3.33 },{ "message":"Hello World", "id": 3.66 }] } }

        }

       /*!



         */

       U1db.Document {
            id: aDocument4
            database: aDatabase
            docId: 'helloworld4'
            defaults:{"hello": { "world": { "message":"Hello World", "id": 4 } } }

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

                        delegate: Text {
                            x: 66; y: 77
                            text: {

                                /*!

                                  A delegate will be created based on particular properties such as the size of the application window, ListView, and delegate itself (amongst other factors). Each delegate can then represent a Document retrieved from the Database based on the record's index. When the number of Documents is less than or equal to the number of delegates then there is a one to one mapping of index to delegate (e.g. the first delegate will represent the Document with an index = 0; the second, index = 1; and so on).

                                  When there are more Documents than delegates the ListView will request a new index depending on the situation (e.g. a user scrolls up or down). For example, if a ListView has 10 delegates, but 32 Documents to handle, when a user initially scrolls the first delegate will change from representing the Document with index = 0 to the Document that might have index = 8; the second, from index = 1 to index = 9; ...; the 10th delegate from index = 9 to index = 17. A second scrolling gesture the first index may change to 15, and the final index 24. And so on. Scrolling in the opposite direction will have a similar effect, but the Document index numbers for each delegate will obviously start to decline (towards their original values).

                                  The following snippet could demonstrate this effect if there were enough Documents to do so (i.e. some number greater than the number of delegates):



                                  \code
                                  delegate: Text {
                                      x: 66; y: 77
                                      text: index
                                  }
                                  \endcode

                                  The object called 'contents' contains one or more properties. This example demonstrates the retrieval of data based on the U1db.Index defined earlier (id: by-helloworld). In this instance the Index contained two expressions simultaniously, "hello.world.id" and "hello.world.message"

                                  \code
                                  delegate: Text {
                                      x: 66; y: 77
                                      text: {
                                           text: "(" + index + ") '" + contents.message + " " + contents.id + "'"
                                      }
                                  }
                                  \endcode

                                */

                                text: "(" + index + ") '" + contents.message + " " + contents.id + "'"
                            }
                        }
                    }
                }

            }

        }

    }

}


