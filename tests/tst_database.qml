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
        path: "aDatabaseB"
        property bool first_row_loaded: false
        property bool last_row_loaded: false
        onDocLoaded: {
            if (path == 'aDatabaseC' && docId == 'dl0')
                first_row_loaded = true
            if (path == 'aDatabaseC' && docId == 'dl99')
                last_row_loaded = true
        }
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

    ListView {
        id: myList
        model: myDatabase
        width: 200; height: 200
        delegate: Text {
            x: 66; y: 77
            text: "otherDelegate index:%1".arg(index)
            // text: "myDelegate index:%1 docId:%2 contents:%3".arg(index).arg(docId).arg(contents)
        }
    }

    ListView {
        id: otherList
        model: firstQuery
        width: 200; height: 200
        delegate: Text {
            x: 66; y: 77
            text: "otherDelegate index:%1".arg(index)
            // text: "otherDelegate index:%1 docId:%2 contents:%3".arg(index).arg(docId).arg(contents)
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

        var myPath = "aDatabaseA"
        myDatabase.path = myPath
        spyPathChanged.wait()
        compare(myDatabase.path, myPath)
        compare(myDatabase.putDoc({"spam": "eggs"}) > -1, true)
        var json = {"foo": "bar"}
        compare(myDatabase.putDoc(json, "hijklmn") > -1, true)
        compare(myDatabase.getDoc("hijklmn"), json)
        compare(myDatabase.getDoc("hijklmn"), json)
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
        myDatabase.putIndex("by-phone-number", ["managers.phone_number"])
        compare(myDatabase.getIndexExpressions('by-phone-number'), ["managers.phone_number"])
        myDatabase.putDoc({ 'managers': [
            { 'name': 'Mary', 'phone_number': '12345' },
            { 'name': 'Rob', 'phone_number': '54321' },
            ] })
        // FIXME compare(myDatabase.getIndexKeys('by-phone-number'), ['12345', '54321'])
    }

    function test_6_fillDocument () {
        var path = "aDatabaseC"
        myDatabase.path = path
        spyPathChanged.wait()
        for (var i = 0; i < 100; i++)
            myDatabase.putDoc({'foo': 'bar'} ,'dl' + Number(i).toLocaleString())
        myDatabase.path = ":memory:"
        spyPathChanged.wait()
        compare(myDatabase.listDocs(), [])
        compare(myList.count, 0)
        myDatabase.first_row_loaded = false
        myDatabase.last_row_loaded = false
        myDatabase.path = path
        spyPathChanged.wait()
        compare(myList.count, 100)
        spyDocLoaded.wait()
        // FIXME compare(myDatabase.first_row_loaded, true)
        // FIXME compare(myDatabase.last_row_loaded, false)
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

    SignalSpy {
        id: spyDocLoaded
        target: myDatabase
        signalName: "docLoaded"
    }
} }

