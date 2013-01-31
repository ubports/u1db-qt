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

#include <QDebug>

#include "database.h"
#include "private.h"

QT_BEGIN_NAMESPACE

namespace U1dbDatabase {

Database::Database(QObject *parent) :
    m_path(""),
    m_count(0)
{
    m_db = QSqlDatabase::addDatabase("SQLITE");
    m_db.setDatabaseName(":memory:");
}

void
Database::setPath(const QString& path)
{
    if (m_path == path)
        return;

    m_path = path;
    qDebug() << "Updated path: " << path;
    Q_EMIT pathChanged(path);

    // TODO: full path
    m_db.setDatabaseName(path);
}

QString
Database::getPath()
{
    return m_path;
}

int
Database::getCount()
{
    return m_count;
}

} // namespace U1dbDatabase

QT_END_NAMESPACE

#include "moc_database.cpp"

