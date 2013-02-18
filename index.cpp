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

QT_BEGIN_NAMESPACE

namespace U1dbDatabase {

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
Index::setDatabase(Database* database)
{
    if (m_database == database)
        return;

    m_database = database;
    Q_EMIT databaseChanged(database);
}

QString
Index::getName()
{
    return m_name;
}

void
Index::setName(const QString& name)
{
    if (m_name == name)
        return;

    m_name = name;
    Q_EMIT nameChanged(name);
}

QVariant
Index::getExpression()
{
    return m_expression;
}

void
Index::setExpression(QVariant expression)
{
    if (m_expression == expression)
        return;

    m_expression = expression;
    Q_EMIT expressionChanged(expression);
}

} // namespace U1dbDatabase

QT_END_NAMESPACE

#include "moc_index.cpp"
