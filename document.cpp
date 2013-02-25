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

#include "document.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

Document::Document(QObject *parent) :
    QObject(parent), m_database(0), m_create(false)
{
}

Database*
Document::getDatabase()
{
    return m_database;
}

void
Document::setDatabase(Database* database)
{
    if (m_database == database)
        return;

    m_database = database;
    Q_EMIT databaseChanged(database);
}

QString
Document::getDocId()
{
    qDebug() << "setting test property";
    setProperty("content", "lala");
    return m_docId;
}

void
Document::setDocId(const QString& docId)
{
    if (m_docId == docId)
        return;

    m_docId = docId;
    Q_EMIT docIdChanged(docId);
}

bool
Document::getCreate()
{
    return m_create;
}

void
Document::setCreate(bool create)
{
    if (m_create == create)
        return;

    m_create = create;
    Q_EMIT createChanged(create);
}

QVariant
Document::getDefaults()
{
    return m_defaults;
}

void
Document::setDefaults(QVariant defaults)
{
    if (m_defaults == defaults)
        return;

    m_defaults = defaults;
    Q_EMIT defaultsChanged(defaults);
}

QT_END_NAMESPACE_U1DB

#include "moc_document.cpp"

