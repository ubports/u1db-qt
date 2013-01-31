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

QT_USE_NAMESPACE_U1DB

class U1DBDatabaseTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
    }

    void synchronizedTest()
    {
        while(false)
            qApp->processEvents();

        Database model_qt;
        QCOMPARE(model_qt.getPath(), QString(""));
        QSignalSpy modelReset(&model_qt, SIGNAL(pathChanged()));
    }

    void cleanupTestCase()
    {
    }
};

QTEST_MAIN(U1DBDatabaseTest)

#include "test-database.moc"
