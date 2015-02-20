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

/*!
    \class Document
    \inmodule U1db
    \ingroup modules

    \brief The Document class proxies a single document stored in the Database.

    This is the declarative API equivalent of Database::putDoc() and
    Database::getDoc().
*/

/*!
    Instantiate a new Document with an optional \a parent,
    usually by declaring it as a QML item.
 */
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
        m_contents = m_database->getDocUnchecked(m_docId);
        Q_EMIT contentsChanged(m_contents);
    }
}

void
Document::onPathChanged(const QString& path)
{
    if (!m_docId.isEmpty())
    {
        m_contents = m_database->getDocUnchecked(m_docId);
        Q_EMIT contentsChanged(m_contents);
    }
}

/*!
    \property Document::database
    The database is used to lookup the contents of the document, reflecting
    changes done to it and conversely changes are saved to the database.
 */
void
Document::setDatabase(Database* database)
{
    if (m_database == database)
        return;

    if (m_database)
        QObject::disconnect(m_database, 0, this, 0);

    m_database = database;
    if (m_database)
    {
        if (!m_docId.isEmpty())
        {
            m_contents = m_database->getDocUnchecked(m_docId);
            Q_EMIT contentsChanged(m_contents);
        }
        QObject::connect(m_database, &Database::pathChanged, this, &Document::onPathChanged);
        QObject::connect(m_database, &Database::docChanged, this, &Document::onDocChanged);
    }
    Q_EMIT databaseChanged(database);
}

QString
Document::getDocId()
{
    return m_docId;
}

/*!
    \property Document::docId
    The docId can be that of an existing document in the database and
    will determine what getContents() returns.
    If no such documents exists, setDefaults() can be used to supply a preset.
 */
void
Document::setDocId(const QString& docId)
{
    if (m_docId == docId)
        return;

    m_docId = docId;
    Q_EMIT docIdChanged(docId);

    if (m_database)
    {
        m_contents = m_database->getDocUnchecked(docId);
        Q_EMIT contentsChanged(m_contents);
    }
}

bool
Document::getCreate()
{
    return m_create;
}

/*!
    \property Document::create
    If create is true, docId is not empty and no document with the same docId
    exists, defaults will be used to store the document.
 */
void
Document::setCreate(bool create)
{
    if (m_create == create)
        return;

    m_create = create;
    Q_EMIT createChanged(create);

    if (m_create && m_database && m_defaults.isValid() && !m_database->getDocUnchecked(m_docId).isValid())
        m_database->putDoc(m_defaults, m_docId);
}

QVariant
Document::getDefaults()
{
    return m_defaults;
}

/*!
    \property Document::defaults
    The default contents of the document, which are used only if
    create is true, docId is not empty and no document with the same
    docId exists in the database yet.
    If the defaults change, it's up to the API user to handle it.
 */
void
Document::setDefaults(QVariant defaults)
{
    if (defaults.canConvert<QVariantMap>())
        defaults = defaults.value<QVariantMap>();

    if (m_defaults == defaults)
        return;

    m_defaults = defaults;
    Q_EMIT defaultsChanged(defaults);

    if (m_create && m_database && m_defaults.isValid() && !m_database->getDocUnchecked(m_docId).isValid())
        m_database->putDoc(m_defaults, m_docId);
}

QVariant
Document::getContents()
{
    return m_contents;
}

/*!
    \property Document::contents
    Updates the contents of the document. A valid docId must be set.
 */
void
Document::setContents(QVariant contents)
{
    if (contents.canConvert<QVariantMap>())
        contents = contents.value<QVariantMap>();

    if (m_contents == contents)
        return;

    m_contents = contents;
    Q_EMIT contentsChanged(contents);
    if (m_database && !m_docId.isEmpty())
        m_database->putDoc(m_contents, m_docId);
}

QT_END_NAMESPACE_U1DB

#include "moc_document.cpp"

