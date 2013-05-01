/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Christian Dywan <christian.dywan@canonical.com>
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
        id: settingsDatabase
        /*
            Uncomment to persistently store settings on disk
        path: "settings.db"
        */
    }

    U1db.Document {
        id: settingsDocument
        database: settingsDatabase
        docId: 'settings'
        create: true
        defaults: { "sound": true, "music": false, "username": "Joe User" }
    }

    Page {
        Tabs {
            Tab {
                title: i18n.tr("Game Settings")
                page: Page {
                    anchors.centerIn: parent
                    Column {
                        spacing: units.gu(2)
                        Row {
                            spacing: units.gu(2)
                            Label {
                                text: i18n.tr("Username")
                                width: units.gu(19)
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextField {
                                placeholderText: settingsDocument.contents.username
                            }
                        }
                        Row {
                            spacing: units.gu(2)
                            Label {
                                text: i18n.tr("Sound")
                                width: units.gu(19)
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Switch {
                                checked: settingsDocument.contents.sound
                            }
                        }
                        Row {
                            spacing: units.gu(2)
                            Label {
                                text: i18n.tr("Music")
                                width: units.gu(19)
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Switch {
                                checked: settingsDocument.contents.music
                            }
                        }
                    }
                }
            }
         }
    }
}

