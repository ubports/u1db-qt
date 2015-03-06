/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *  Marco Trevisan <marco.trevisan@canonical.com>
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
    width: units.gu(45)
    height: units.gu(80)

        U1db.Database {
            id: aDatabase
        }

       U1db.Document {
            database: aDatabase
            docId: "department"
            contents: {"department": "department of redundancy department",
                       "managers": [
                            {"name": "Mary", "phone_number": "12345"},
                            {"name": "Katherine"},
                            {"name": "Rob", "phone_number": "54321"}
                        ]
                      }
      }

       U1db.Index{
           database: aDatabase
           id: by_phonenumber
           expression: ["managers.phone_number"]
       }

       U1db.Query{
           id: aQuery
           index: by_phonenumber
       }

        Tabs {
            id: tabs

            Tab {
                title: i18n.tr("Hello U1Db!")

                page: Page {
                    id: helloPage

                    ListView {
                        anchors.fill: parent
                        model: aQuery
                        delegate: Text {
                            text: "(" + index + ") " + contents.phone_number
                    }
                }
            }
        }
    }
}

