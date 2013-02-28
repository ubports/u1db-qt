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
    
    U1db.Database {
        id: aDatabase
        path: "aU1DbDSatabase"
    }
    
    U1db.Document {
        id: aDocument
        database: aDatabase
        docId: 'helloworld'
        create: true
        defaults: { "hello": "world" }
    }
    
    Tabs {
        id: tabs
        anchors.fill: parent

        Tab {
            objectName: "Tab1"

            title: i18n.tr("Hello U1Db!")

            page: Page {
											    ListView {
                    width: units.gu(45)
                    height: units.gu(80)
                    model: aDatabase
       
                    delegate: Text {
                        x: 66; y: 77
                        text: {
                            console.info("aDelegate index:%1 docId:%2 contents:%3".arg(index).arg(docId).arg(contents))
                            ""
                        }
                    }
                }
            }
            
            
     }
    
    
     }

}