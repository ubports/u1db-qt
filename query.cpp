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

/*!
    \class Query
    \inmodule U1db

    \brief The Query class generates a filtered list of documents based on either
    a query or a range, and using the given Index.

    This is the declarative API equivalent of FIXME
*/

Query::Query(QObject *parent) :
    QAbstractListModel(parent), m_index(0)
{
}

/*!
 * \brief Query::data
 * \param index
 * \param role
 * \return
 *Used to implement QAbstractListModel
 *Implements the variables exposed to the Delegate in a model
 */
QVariant
Query::data(const QModelIndex & index, int role) const
{
    QVariantMap result(m_hash.value(index.row()));

    if (role == 0) // contents
    {
        Database* db(m_index->getDatabase());
        if (db)
        {
            return result;
        }
    }
    if (role == 1) // docId
        //return docId;
    return QVariant();
}

/*!
    Used to implement QAbstractListModel
    Defines \b{contents} and \b{docId} as variables exposed to the Delegate in a model
    \b{index} is supported out of the box.
 */
QHash<int, QByteArray>
Query::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(0, "contents");
    roles.insert(1, "docId");
    return roles;
}

/*!
    Used to implement QAbstractListModel
    The number of rows: the number of documents given by the query.
 */
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

    Database *db = m_index->getDatabase();
    if(db){
        if(db->documentCount>0){
            QObject::connect(db, &Database::documentsAvailable, this, &Query::onDataInvalidated);
        }
    }

    m_index->clearResults();

    m_index->generateIndexResults();

    QListIterator<QVariantMap> i(m_index->getAllResults());

    while (i.hasNext()) {
        QVariantMap result = i.next();
        m_hash.insert(m_hash.count(),result);
    }

}

/*!
    Sets the Index to use. The index must have a valid name and index expressions,
    then either a range or query can be set.
 */
void
Query::setIndex(Index* index)
{
    if (m_index == index)
        return;

    if (m_index)
        QObject::disconnect(m_index, 0, this, 0);
    m_index = index;
    if (m_index){
        QObject::connect(m_index, &Index::dataInvalidated, this, &Query::onDataInvalidated);
        QObject::connect(m_index, &Index::dataIndexed, this, &Query::onDataInvalidated);
    }
    Q_EMIT indexChanged(index);


    onDataInvalidated();

}

QVariant
Query::getQuery()
{
    return m_query;
}

/*!
    Sets a range, such as ['match', false].
    Only one of query and range is used - setting range unsets the query.
 */
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

/*!
    Sets a range, such as [['a', 'b'], ['*']].
    Only one of query and range is used - setting range unsets the query.
 */
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

