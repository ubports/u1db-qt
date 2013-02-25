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

#ifndef U1DB_QUERY_H
#define U1DB_QUERY_H

#include <QtCore/QObject>
#include <QSqlDatabase>
#include <QVariant>

#include "database.h"
#include "index.h"

QT_BEGIN_NAMESPACE_U1DB

class Q_DECL_EXPORT Query : public QObject {
    Q_OBJECT
    Q_PROPERTY(QT_PREPEND_NAMESPACE_U1DB(Database*) database READ getDatabase WRITE setDatabase NOTIFY databaseChanged)
    Q_PROPERTY(QT_PREPEND_NAMESPACE_U1DB(Index*) index READ getIndex WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(QVariant query READ getQuery WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QVariant range READ getRange WRITE setRange NOTIFY rangeChanged)
public:
    Query(QObject* parent = 0);
    ~Query() { }

    Database* getDatabase();
    void setDatabase(Database* database);
    Index* getIndex();
    void setIndex(Index* index);
    QVariant getQuery();
    void setQuery(QVariant query);
    QVariant getRange();
    void setRange(QVariant range);
Q_SIGNALS:
    void databaseChanged(Database* database);
    void indexChanged(Index* index);
    void queryChanged(QVariant query);
    void rangeChanged(QVariant range);
private:
    Q_DISABLE_COPY(Query)
    Database* m_database;
    Index* m_index;
    QVariant m_query;
    QVariant m_range;
};

QT_END_NAMESPACE_U1DB

#endif // U1DB_QUERY_H

