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

#include "index.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

/*!
    \class Index
    \inmodule U1db

    \brief The Index class defines an index to be stored in the database and
    queried using Query. Changes in documents affected by the index also update
    the index in the database.

    This is the declarative API equivalent of Database::putIndex() and
    Database::getIndexExpressions().
*/

Index::Index(QObject *parent) :
    QObject(parent), m_database(0)
{
}

Database*
Index::getDatabase()
{
    return m_database;
}

void
Index::onPathChanged(const QString& path)
{
    Q_EMIT dataInvalidated();
}

void
Index::onDocChanged(const QString& docId, QVariant content)
{
    Q_EMIT dataInvalidated();
}

/*!
    Sets the Database to lookup documents from and store the index in. The
    dataInvalidated() signal will be emitted on all changes that could affect
    the index.
 */
void
Index::setDatabase(Database* database)
{
    if (m_database == database)
        return;

    if (m_database)
        QObject::disconnect(m_database, 0, this, 0);

    m_database = database;
    Q_EMIT databaseChanged(database);

    if (m_database)
    {
        m_database->putIndex(m_name, m_expression);
        QObject::connect(m_database, &Database::pathChanged, this, &Index::onPathChanged);
        QObject::connect(m_database, &Database::docChanged, this, &Index::onDocChanged);
        Q_EMIT dataInvalidated();
    }
}

QString
Index::getName()
{
    return m_name;
}

/*!
    Sets the name used. Both an expression and a name must be specified
    for an index to be created.
 */
void
Index::setName(const QString& name)
{
    if (m_name == name)
        return;

    if (m_database)
    {
        m_database->putIndex(name, m_expression);
        Q_EMIT dataInvalidated();
    }

    m_name = name;
    Q_EMIT nameChanged(name);
}

QStringList
Index::getExpression()
{
    return m_expression;
}

/*!
    Sets the expression used. Both an expression and a name must be specified
    for an index to be created.
 */
void
Index::setExpression(QStringList expression)
{
    if (m_expression == expression)
        return;

    if (m_database)
    {
        m_database->putIndex(m_name, expression);
        Q_EMIT dataInvalidated();
    }

    m_expression = expression;
    Q_EMIT expressionChanged(expression);
}

QT_END_NAMESPACE_U1DB

#include "moc_index.cpp"

