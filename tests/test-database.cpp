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

#include <QtTest>
#include <QObject>

#include "database.h"
#include "document.h"
#include "index.h"
#include "query.h"

QT_USE_NAMESPACE_U1DB

class U1DBDatabaseTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        while(false)
            qApp->processEvents();
    }

    void testCanSetPath()
    {
        Database db;
        QCOMPARE(db.getPath(), QString(""));
        QSignalSpy modelReset(&db, SIGNAL(pathChanged(const QString&)));
        QTemporaryFile file;
        QCOMPARE(file.open(), true);
        db.setPath(file.fileName());
        QCOMPARE(db.getPath(), file.fileName());
        QVERIFY(db.lastError().isEmpty());
    }

    void testCanSetEmptyPath()
    {
        Database db;
        QCOMPARE(db.getPath(), QString());
        QSignalSpy modelReset(&db, SIGNAL(pathChanged(const QString&)));
        QTemporaryFile file;
        QCOMPARE(file.open(), true);
        db.setPath(file.fileName());
        QCOMPARE(db.getPath(), file.fileName());
        db.setPath("");
        QCOMPARE(db.getPath(), QString());
        QVERIFY(db.lastError().isEmpty());
    }

    void testCanSetPathUsingQUrl()
    {
        Database db;
        QCOMPARE(db.getPath(), QString(""));
        QSignalSpy modelReset(&db, SIGNAL(pathChanged(const QString&)));
        QTemporaryFile file;
        QCOMPARE(file.open(), true);
        QString url = QUrl::fromLocalFile(file.fileName()).toString();
        db.setPath(url);
        QCOMPARE(db.getPath(), url);
    }

    void testNonExistingParentFolder()
    {
        Database db;
        QTemporaryFile file("spamXXXXXX");
        file.setAutoRemove(false);
        QCOMPARE(file.open(), true);
        QString subfolder(file.fileName() + "/eggs");
        QFile::remove(file.fileName());
        db.setPath(subfolder);
        QCOMPARE(db.getPath(), subfolder);
        QVERIFY(db.lastError().isEmpty());
    }

    void testCanSetIndex()
    {
        Database db;
        Index index;
        index.setDatabase(&db);
        index.setName("py-name-phone");
        index.setExpression(QStringList() << "gents.name" << "gents.phone");
    }

    void testCanSetQuery()
    {
        Database db;
        Index index;
        index.setDatabase(&db);
        index.setName("by-date");
        index.setExpression(QStringList() << "date" << "sports" << "software");

        Query query;
        query.setIndex(&index);
        query.setQuery(QStringList() << "2014*" << "basketball" << "linux");
    }

    void testSingleDocumentQuery()
    {
        const char * json = "{\"department\": \"department of redundancy department\"," \
                            " \"managers\": [" \
                            "    {\"name\": \"Mary\", \"phone_number\": \"12345\"}," \
                            "    {\"name\": \"Katherine\"}," \
                            "    {\"name\": \"Rob\", \"phone_number\": [\"54321\"]}" \
                            "  ]" \
                            "}";
        Database db;
        Document doc;
        doc.setDocId("department");
        doc.setDatabase(&db);
        doc.setContents(QJsonDocument::fromJson(QByteArray(json)).toVariant());

        Index index;
        index.setDatabase(&db);
        index.setName("by-phone-number");
        index.setExpression(QStringList() << "managers.phone_number");

        Query query;
        query.setIndex(&index);

        QCOMPARE(query.getDocuments().size(), 1);
        QCOMPARE(query.getDocuments().front(), QString("department"));

        QList<QVariant> expected_numbers;
        Q_FOREACH (QVariant manager, doc.getContents().toMap()["managers"].toList())
        {
            QVariantMap man = manager.toMap();
            if (man.keys().contains("phone_number"))
            {
                man.remove("name");
                expected_numbers.append(man);
            }
        }

        QCOMPARE(query.getResults(), expected_numbers);
    }

    void cleanupTestCase()
    {
    }
};

QTEST_MAIN(U1DBDatabaseTest)

#include "test-database.moc"
