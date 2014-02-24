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
        path: 'aDatabaseU'
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
        contents: { 'misc': { 'software': 'linux', 'sports': [ 'basketball', 'hockey' ] }, 'date': '2014-01-01' , 'gents': [ { 'name': 'Ivanka', 'phone': 00321 }, ] }
    }

    U1db.Document {
        database: gents
        docId: 'F'
        contents: { 'details': { 'name': 'spy', 'type': 'hide', 'colour': 'blue' } }
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

    U1db.Index {
        id: byDate
        database: gents
        name: 'by-date'
        expression: ['date', 'sports', 'software']
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
        query: [ { 'phone': '*' } ]
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
        query: [ { 'name': 'Ivanka', 'phone': '*' } ]
    }

    U1db.Query {
        id: wrongQuery
        index: byNamePhone
        query: [{ 'name': 'Ivanka', 'phone': '*' }]
    }

    U1db.Query {
        id: toplevelQuery
        index: byDate
        query: [{ 'date': '2014*', 'sports': 'basketball', 'software': 'linux' }]
    }

    U1db.Query {
        id: queryOne
        index: U1db.Index {
            database: gents
            expression: [ 'details.type' ]
        }
        query: [ 'show' ]
    }

    U1db.Query {
        id: queryBoth
        index: U1db.Index {
            database: gents
            expression: [ 'details.type', 'details.colour' ]
        }
        query: [ 'show', '*' ]
    }

    SignalSpy {
        id: spyDocumentsChanged
        target: defaultPhone
        signalName: "documentsChanged"
    }

TestCase {
    name: "U1dbDatabase"
    when: windowShown

    function prettyJson(j) {
        var A = JSON.stringify(j)
        if (A['0'] && A != '{}') {
            var A = '['
            for(var i in j)
                A += JSON.stringify(j[i]) + ','
            A = A.substring(0, A.lastIndexOf(',')) + ']'
        }
        return A
    }

    function compare (a, b, msg) {
        /* Override built-in compare to:
           Match different JSON for identical values (number hash versus list)
           Produce readable output for all JSON values
         */
        if (a == b)
            return
        var A = prettyJson(a), B = prettyJson(b)
        if (A != B) {
            fail('%5%1 != %2 (%3 != %4)'.arg(A).arg(B).arg(JSON.stringify(a)).arg(JSON.stringify(b)).arg(msg ? msg + ': ' : ''))
        }
    }

    function workaroundQueryAndWait (buggyQuery) {
        var realQuery = buggyQuery.query;
        spyDocumentsChanged.target = buggyQuery
        spyDocumentsChanged.wait();
    }

    function test_0_wrongUse () {
        workaroundQueryAndWait(wrongQuery)
        ignoreWarning('u1db: Unexpected type QVariantMap for query')
        wrongQuery.query = { 'name': 'Ivanka' }
        ignoreWarning('u1db: Unexpected type QObject* for query')
        wrongQuery.query = defaultPhone
    }

    function test_1_defaults () {
        // We should get all documents
        workaroundQueryAndWait(defaultPhone)
        compare(defaultPhone.documents, ['1', '_', 'a'], 'uno')
        compare(defaultPhone.results.length, 3, 'dos')
        compare(defaultPhone.results.length, defaultPhone.documents.length, 'puntos')
        // FIXME: compare(defaultPhone.results, [], 'dos')
        // These queries are functionally equivalent
        compare(defaultPhone.documents, allPhone.documents, 'tres')
        compare(defaultPhone.documents, allPhoneList.documents, 'quatro')
        workaroundQueryAndWait(allPhoneKeywords)
        compare(defaultPhone.documents, allPhoneKeywords.documents, 'cinco')
        // Results are also equivalent
        compare(defaultPhone.results.length, allPhoneKeywords.results.length , 'siete')
        compare(defaultPhone.results, allPhoneKeywords.results, 'seis')
    }

    function test_2_numbers () {
        // We should get '1'
        compare(s12345Phone.documents, ['1'], 'uno')
        // It's okay to mix strings and numerical values
        compare(s12345Phone.documents, i12345Phone.documents, 'dos')
    }

    function test_3_wildcards () {
        // Trailing string wildcard
        compare(s1wildcardPhone.documents, ['1'], 'uno')
        // Last given field can use wildcards
        // FIXME: compare(ivankaAllNamePhone.documents, ['_', 'a'], 'dos')
        // These queries are functionally equivalent
        workaroundQueryAndWait(ivankaAllNamePhoneKeywords)
        workaroundQueryAndWait(ivankaAllNamePhone)
        // FIXME: compare(ivankaAllNamePhone.documents, ivankaAllNamePhoneKeywords.documents, 'tres')
        compare(toplevelQuery.documents, ['_'])
    }

    function test_4_delete () {
        compare(defaultPhone.documents, ['1', '_', 'a'], 'uno')
        // Deleted aka empty documents should not be returned
        gents.deleteDoc('_')
        compare(defaultPhone.documents, ['1', 'a'], 'dos')
    }

    function test_5_fields () {
        compare(queryOne.documents, {}, 'one field')
        compare(queryBoth.documents, {}, 'two fields')
    }
} }

