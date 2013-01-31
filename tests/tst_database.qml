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
import QtTest 1.0
import U1db 1.0 as U1db

TestCase {
    name: "U1dbDatabase"

    function test_1_databasePopulated () {
        myDatabase.path = "notes"
        spyPathChanged.wait()
        compare(myDatabase.path, "notes")
    }

    SignalSpy {
        id: spyPathChanged
        target: myDatabase
        signalName: "pathChanged"
    }

    U1db.Database {
        id: myDatabase
        path: "bogus"
    }

    /*
    U1db.Document {
        id: myDocument
        database: myDatabase
        documentId: 'my-document-id'
        defaults: { "eggs": "spam" }
    }

    ListView {
        id: myList
        model: myDocument
        delegate: Text {
            text: eggs
        }
    */
}

