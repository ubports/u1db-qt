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
        id: gents
    }

    U1db.Document {
        database: gents
        docId: '1'
        contents: { 'gents': [ { 'name': 'Mary', 'phone': 12345 }, { 'name': 'Rob', 'phone': 54321 }, ] }
    }

    U1db.Document {
        database: gents
        docId: 'a'
        contents: { 'gents': [ { 'name': 'George', 'phone': 'NA' }, { 'name': 'Ivanka', 'phone': 50243 }, ] }
    }

    U1db.Document {
        database: gents
        docId: '_'
        contents: { 'gents': [ { 'name': 'Ivanka', 'phone': 00321 }, ] }
    }

    U1db.Index {
        id: byPhone
        database: gents
        name: 'by-phone'
        expression: ['gents.phone']
    }

    U1db.Index {
        id: byNamePhone
        database: gents
        name: 'by-name-phone'
        expression: ['gents.name', 'gents.phone']
    }

    U1db.Query {
        id: defaultPhone
        index: byPhone
    }

    U1db.Query {
        id: allPhone
        index: byPhone
        query: '*'
    }

    U1db.Query {
        id: allPhoneList
        index: byPhone
        query: ['*']
    }

    U1db.Query {
        id: allPhoneKeywords
        index: byPhone
        query: { 'phone': '*' }
    }

    U1db.Query {
        id: s12345Phone
        index: byPhone
        query: '12345'
    }

    U1db.Query {
        id: i12345Phone
        index: byPhone
        query: 12345
    }

    U1db.Query {
        id: s1wildcardPhone
        index: byPhone
        query: '1*'
    }

    U1db.Query {
        id: ivankaAllNamePhone
        index: byNamePhone
        query: ['Ivanka', '*']
    }

    U1db.Query {
        id: ivankaAllNamePhoneKeywords
        index: byNamePhone
        query: { 'name': 'Ivanka', 'phone': '*' }
    }

TestCase {
    name: "U1dbDatabase"
    when: windowShown

    function test_1_defaults () {
        // We should get all documents
        compare(defaultPhone.results, [])
        // Results are also equivalent
        compare(defaultPhone.results, allPhoneKeywords.results)
    }
} }

