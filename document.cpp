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
Document::onDocChanged(const QString& docId, QVariant content)
{
    if (docId == m_docId)
    {
        m_contents = m_database->getDoc(m_docId);
        Q_EMIT contentsChanged(m_contents);
    }
}

void
Document::onPathChanged(const QString& path)
{
    if (!m_docId.isEmpty())
    {
        m_contents = m_database->getDoc(m_docId);
        Q_EMIT contentsChanged(m_contents);
    }
}

void
Document::setDatabase(Database* database)
{
    if (m_database == database)
        return;

    if (m_database)
        QObject::disconnect(m_database, 0, this, 0);

    if (m_database && !m_docId.isEmpty())
        m_contents = m_database->getDoc(m_docId);

    m_database = database;
    QObject::connect(m_database, &Database::pathChanged, this, &Document::onPathChanged);
    QObject::connect(m_database, &Database::docChanged, this, &Document::onDocChanged);
    Q_EMIT databaseChanged(database);
}

QString
Document::getDocId()
{
    return m_docId;
}

void
Document::setDocId(const QString& docId)
{
    if (m_docId == docId)
        return;

    m_docId = docId;
    Q_EMIT docIdChanged(docId);

    if (m_database)
    {
        m_contents = m_database->getDoc(docId);
        Q_EMIT contentsChanged(m_contents);
    }
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

QVariant
Document::getContents()
{
    return m_contents;
}

void
Document::setContents(QVariant contents)
{
    if (m_contents == contents)
        return;

    m_contents = contents;
    Q_EMIT contentsChanged(contents);
}

QT_END_NAMESPACE_U1DB

#include "moc_document.cpp"

