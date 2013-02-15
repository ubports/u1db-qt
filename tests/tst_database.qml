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
        var myPath = "/tmp/u1db-qt.db"
        myDatabase.path = myPath
        spyPathChanged.wait()
        compare(myDatabase.path, myPath)
        var docId = myDatabase.putDoc("hijklmn", {"foo": "bar"})
        console.log("putDoc: " + docId)
        console.log("getDoc(0): " + myDatabase.getDoc(docId, false))
        console.log("getDoc(1): " + myDatabase.getDoc(docId, true))
        console.log("listDocs: " + myDatabase.listDocs())
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

    U1db.Document {
        id: myDocument
        database: myDatabase
        docId: 'my-document-id'
        create: true
        defaults: { "eggs": "spam" }
    }

    U1db.Index {
        id: myIndex
        database: myDatabase
        name: 'by-title-field'
        expression: ['title', 'bool(field)']
    }

    U1db.Query {
        id: firstQuery
        database: myDatabase
        index: myIndex
        query: ['match', false]
    }

    U1db.Query {
        id: secondQuery
        database: u1db
        index: otherIndex
        range: [['a', 'b'], ['*']]
    }

    /*
    ListView {
        id: myList
        model: myDocument
        delegate: Text {
            text: eggs
        }
    */
}

