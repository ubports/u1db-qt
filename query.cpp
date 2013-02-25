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
#include <QSqlQuery>
#include <QFile>
#include <QSqlError>
#include <QUuid>
#include <QStringList>
#include <QJsonDocument>

#include "query.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

Query::Query(QObject *parent) :
    QObject(parent), m_database(0), m_index(0)
{
}

Database*
Query::getDatabase()
{
    return m_database;
}

void
Query::setDatabase(Database* database)
{
    if (m_database == database)
        return;

    m_database = database;
    Q_EMIT databaseChanged(database);
}

Index*
Query::getIndex()
{
    return m_index;
}

void
Query::setIndex(Index* index)
{
    if (m_index == index)
        return;

    m_index = index;
    Q_EMIT indexChanged(index);
}

QVariant
Query::getQuery()
{
    return m_query;
}

void
Query::setQuery(QVariant query)
{
    if (m_query == query)
        return;

    if (m_range.isValid())
        m_range = QVariant();

    m_query = query;
    Q_EMIT queryChanged(query);
}

QVariant
Query::getRange()
{
    return m_range;
}

void
Query::setRange(QVariant range)
{
    if (m_range == range)
        return;

    if (m_query.isValid())
        m_query = QVariant();

    m_range = range;
    Q_EMIT rangeChanged(range);
}

QT_END_NAMESPACE_U1DB

#include "moc_query.cpp"

