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
#include "database.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

Query::Query(QObject *parent) :
    QAbstractListModel(parent), m_index(0)
{
}

QVariant
Query::data(const QModelIndex & index, int role) const
{
    QString docId(m_hash.value(index.row()));
    if (role == 0) // contents
    {
        Database* db(m_index->getDatabase());
        if (db)
            return db->getDocUnchecked(docId);
    }
    if (role == 1) // docId
        return docId;
    return QVariant();
}

QHash<int, QByteArray>
Query::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(0, "contents");
    roles.insert(1, "docId");
    return roles;
}

int
Query::rowCount(const QModelIndex & parent) const
{
    return m_hash.count();
}

Index*
Query::getIndex()
{
    return m_index;
}

void
Query::onDataInvalidated()
{
    m_hash.clear();
    Database* db(m_index->getDatabase());
    if (db)
        ;
    // TODO
}

void
Query::setIndex(Index* index)
{
    if (m_index == index)
        return;

    if (m_index)
        QObject::disconnect(m_index, 0, this, 0);
    m_index = index;
    if (m_index)
        QObject::connect(m_index, &Index::dataInvalidated, this, &Query::onDataInvalidated);
    Q_EMIT indexChanged(index);
    onDataInvalidated();
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
    onDataInvalidated();
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
    onDataInvalidated();
}

QT_END_NAMESPACE_U1DB

#include "moc_query.cpp"

