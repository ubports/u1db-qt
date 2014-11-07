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
        db.setPath(file.fileName());
        QCOMPARE(db.getPath(), file.fileName());
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

    void cleanupTestCase()
    {
    }
};

QTEST_MAIN(U1DBDatabaseTest)

#include "test-database.moc"
