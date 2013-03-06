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

    id: u1dbView
    width: units.gu(45)
    height: units.gu(80)
    
    /*!
    
    A Database is very simple to create. It only needs an id and a path where the file will be created. A Database is a model, which can be used by elements, such as the ListView further in this example. 
    
    */
    
    U1db.Database {
        id: aDatabase
        path: "aU1DbDSatabase2"
    }
        
    Timer {
    
        property int i: 0; interval: 5; running: true; repeat: true
        onTriggered: newDocumentObject() 
     
        function newDocumentObject() {

           var qmlString = "import QtQuick 2.0; import U1db 1.0 as U1db; U1db.Document {id: aDcoument"+i+";database: aDatabase;docId: 'helloworld"+i+"';create: true; defaults: { 'hello': 'Hello New Document "+i+"!' }}"

           Qt.createQmlObject(qmlString, u1dbView, "dynamicNewDocument"+i);

            i = i+1
         }     
     
    }
    
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
                    model: aDatabase
       
                   /* A delegate will be created for each Document retrieved from the Database */
                    delegate: Text {
                        x: 66; y: 77
                        text: {
                            /*!
                                The object called 'contents' contains a string as demonstrated here. In this example 'hello' is our search string.

                                if(contents !== undefined){
                                    text: contents.hello
                                }
                                else { "" }


                            */

                            if(contents !== undefined){
                                text: contents.hello
                            }
                            else { "" }
                        }
                    }
                }
            }
            
            
     }
    
    
     }

}
