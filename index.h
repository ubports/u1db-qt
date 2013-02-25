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

#ifndef U1DB_INDEX_H
#define U1DB_INDEX_H

#include <QtCore/QObject>
#include <QSqlDatabase>
#include <QVariant>

#include "database.h"

QT_BEGIN_NAMESPACE_U1DB

class Q_DECL_EXPORT Index : public QObject {
    Q_OBJECT
    Q_PROPERTY(QT_PREPEND_NAMESPACE_U1DB(Database*) database READ getDatabase WRITE setDatabase NOTIFY databaseChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant expression READ getExpression WRITE setExpression NOTIFY expressionChanged)
public:
    Index(QObject* parent = 0);
    ~Index() { }

    Database* getDatabase();
    void setDatabase(Database* database);
    QString getName();
    void setName(const QString& name);
    QVariant getExpression();
    void setExpression(QVariant expression);
Q_SIGNALS:
    void databaseChanged(Database* database);
    void nameChanged(const QString& name);
    void expressionChanged(QVariant expression);
private:
    Q_DISABLE_COPY(Index)
    Database* m_database;
    QString m_name;
    QVariant m_expression;
};

QT_END_NAMESPACE_U1DB

#endif // U1DB_INDEX_H

