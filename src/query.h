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
#include <QVariant>

#include "index.h"

QT_BEGIN_NAMESPACE_U1DB

class Q_DECL_EXPORT Query : public QAbstractListModel {
    Q_OBJECT
#ifdef Q_QDOC
    Q_PROPERTY(Index* index READ getIndex WRITE setIndex NOTIFY indexChanged)
#else
    Q_PROPERTY(QT_PREPEND_NAMESPACE_U1DB(Index*) index READ getIndex WRITE setIndex NOTIFY indexChanged)
#endif
    Q_PROPERTY(QVariant query READ getQuery WRITE setQuery NOTIFY queryChanged)
public:
    Query(QObject* parent = 0);

    // QAbstractListModel
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray>roleNames() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    Index* getIndex();
    void setIndex(Index* index);
    QVariant getQuery();
    void setQuery(QVariant query);

    void generateQueryResults();
    bool iterateQueryList(QVariant query, QString field, QString value);
    bool queryString(QString query, QString value);
    bool queryMap(QVariantMap map, QString value, QString field);
    bool queryField(QString field, QVariant value);

Q_SIGNALS:
    void indexChanged(Index* index);
    void queryChanged(QVariant query);
private:
    Q_DISABLE_COPY(Query)
    Index* m_index;
    QHash<int, QVariantMap> m_hash;
    QVariant m_query;

    void onDataInvalidated();
};

QT_END_NAMESPACE_U1DB

#endif // U1DB_QUERY_H

