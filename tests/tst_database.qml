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

Item {
    width: 200; height: 200

    U1db.Database {
        id: myDatabase
        path: "bogus"
    }

    U1db.Document {
        id: myDocument
        database: myDatabase
        docId: 'qwertzui'
        defaults: { "eggs": "spam" }
    }

    U1db.Document {
        id: otherDocument
        database: myDatabase
        docId: 'shallow'
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
        database: myDatabase
        index: myIndex
        range: [['a', 'b'], ['*']]
    }

    ListView {
        id: myList
        model: myDatabase
        width: 200; height: 200
        Component.onCompleted:
            console.info("myList Component.loaded " + myDatabase.path + " " + myDatabase.listDocs())
        delegate: Text {
            x: 66; y: 77
            text: {
                console.info("myDelegate index:%1 docId:%2 contents:%3".arg(index).arg(docId).arg(contents))
                ""
            }
        }
    }

    ListView {
        id: otherList
        model: firstQuery
        width: 200; height: 200
        Component.onCompleted:
            console.info("otherList Component.loaded " + myDatabase.path + " " + myDatabase.listDocs())
        delegate: Text {
            x: 66; y: 77
            text: {
                console.info("otherDelegate")
                ""
            }
        }
    }

TestCase {
    name: "U1dbDatabase"
    when: windowShown

    function test_0_documentCreate () {
        compare(myDatabase.getDoc(otherDocument.docId), otherDocument.defaults)
    }

    function test_1_databasePopulated () {
        spyListCompleted.wait()
        compare(myDatabase.putDoc({"animals": ["cat", "dog", "hamster"]}) > -1, true)
        console.log("listDocs: " + myDatabase.listDocs())

        var myPath = "/tmp/u1db-qt.db"
        myDatabase.path = myPath
        spyPathChanged.wait()
        compare(myDatabase.path, myPath)
        compare(myDatabase.putDoc({"spam": "eggs"}) > -1, true)
        var json = {"foo": "bar"}
        compare(myDatabase.putDoc(json, "hijklmn") > -1, true)
        compare(myDatabase.getDoc("hijklmn"), json)
        compare(myDatabase.getDoc("hijklmn"), json)
        console.log("listDocs: " + myDatabase.listDocs())
        console.log(firstQuery.query)
    }

    function test_2_databaseError () {
        myDatabase.putDoc({"": ""}, "日本語")
        spyErrorChanged.wait()
        compare(myDatabase.error.indexOf("Invalid docID") > -1, true)
    }

    function test_3_documentContents () {
        var json = {"content": {"notetext": "Lorem ipsum"}}
        myDatabase.putDoc(json, "qwertzui")
        myDocument.docId = ''
        compare(myDocument.contents, undefined)
        myDocument.docId = 'qwertzui'
        compare(myDocument.contents, json)
        compare(myDocument.contents.content.notetext, 'Lorem ipsum')

        var path = myDatabase.path
        myDatabase.path = ':memory:'
        myDatabase.path = path
        spyContentsChanged.wait()
    }

    function test_4_putIndex () {
        myDatabase.putIndex("by-foo", ["foo", "bool(field)"])
    }

    SignalSpy {
        id: spyPathChanged
        target: myDatabase
        signalName: "pathChanged"
    }

    SignalSpy {
        id: spyContentsChanged
        target: myDocument
        signalName: "contentsChanged"
    }

    SignalSpy {
        id: spyErrorChanged
        target: myDatabase
        signalName: "errorChanged"
    }

    SignalSpy {
        id: spyListCompleted
        target: myDatabase.Component
        signalName: "completed"
    }

} }

