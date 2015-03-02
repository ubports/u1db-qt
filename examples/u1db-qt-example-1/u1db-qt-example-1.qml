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
        path: Qt.resolvedUrl("aDatabase1");
    }
    
    /*!
    
    A Document can be declared at runtime. It requires at the very least a unique 'docId', but that alone won't do anything special. In order for a document to be entered into the database the below snippet demonstrates the basic requirements. The id might be optional.
    
    
    */
    
    U1db.Document {
        id: aDocument
        database: aDatabase
        docId: 'helloworld'
        create: true
        defaults: { "hello": "Hello World!" }
    }
           
    Tabs {
        id: tabs
        anchors.fill: parent

        Tab {
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
