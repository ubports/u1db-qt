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

This example demonstrates how to create and query one level of sub-fields in a document.

*/

MainView {

    id: u1dbView
    width: units.gu(45)
    height: units.gu(80)

    U1db.Database {
        id: aDatabase
        path: Qt.resolvedUrl("aDatabase2b")
    }
    
    /*!
    
    This snippet demonstrates how to create content that includes nested fields. The main field is 'hello', while the sub-field for each entry is 'value'. Later in the example it will be shown how to access each of these in the delegate of a ListView.
    
    U1db.Document {
        id: aDocument
        database: aDatabase
        docId: 'hello'
        create: true
        defaults: { "hello": [{"value":"99 Hello Worlds on the wall...!"},{"value":"98 Hello Worlds on the wall...!"},{"value":"97 Hello Worlds on the wall...!"},{"value":"...and so on..."}] }
        }
    
    */

    U1db.Document {
        id: aDocument
        database: aDatabase
        docId: 'hello'
        create: true
        defaults: { "hello": [{"value":"99 Hello Worlds on the wall...!"},{"value":"98 Hello Worlds on the wall...!"},{"value":"97 Hello Worlds on the wall...!"},{"value":"...and so on..."}] }
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
               	anchors.fill: parent
               	model: aDocument.contents.hello
               	
               	/*!
               	
               	Determining the current record is easy. All that is required is to access it using the delegate's own 'index' value, as shown here:
               	
               	delegate: Text {
               		height: 30
               		text: aDocument.contents.hello[index].value 
               	}
               	
               	
               	Remember that when the entries were created the sub-field was 'value'. So where index = 0, 'aDocument.contents.hello[0].value' will produce '99 Hello Worlds on the wall...!'. Each entry in the document will in turn create its own delegate with a new index number, which can then be used to extract the 'value' (or whatever other sub-field has been created).         
               	
               	*/
             
               	delegate: Text {
               		height: 30
               		text: aDocument.contents.hello[index].value        
               	}
               
               }
               
            }
               
        }
        

    }
}

