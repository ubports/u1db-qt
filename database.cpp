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
#include <QJsonObject>

#include "database.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

/*!
    \class Database

    \brief The Database class implements the on-disk storage of an individual
    U1DB database.

    The functional API can be used from C++ and Javascript, and is the basis of
    the declarative API.
*/

/*!
    A unique identifier for the state of synchronization
 */
QString
Database::getReplicaUid()
{
    QSqlQuery query (m_db.exec("SELECT value FROM u1db_config WHERE name = 'replica_uid'"));
    if (!query.lastError().isValid() && query.next())
        return query.value(0).toString();
    return setError(QString("Failed to get replica UID: %1\n%2").arg(query.lastError().text()).arg(query.lastQuery())) ? QString() : QString();
}

/*!
    Checks if the underlying SQLite database is ready to be used
    Only to be used as a utility function by initializeIfNeeded()
 */
bool
Database::isInitialized()
{
    m_db.exec("PRAGMA case_sensitive_like=ON");
    QSqlQuery query(m_db.exec(
        "SELECT value FROM u1db_config WHERE name = 'sql_schema'"));
    return query.next();
}

/*!
    Describes the error as a string if the last operation failed.
 */
bool
Database::setError(const QString& error)
{
    qDebug() << "u1db: " << error;
    m_error = error;
    Q_EMIT errorChanged(error);
    return false;
}

/*!
    Describes the error as a string if the last operation failed.
 */
QString
Database::lastError()
{
    return m_error;
}

/*!
    Ensures that the underlying database works, or tries to set it up:

    The SQlite backend is loaded - it's an optional Qt5 module and can fail
    If @path is an existing database, it'll be opened
    For a new database, the default schema will be applied
 */
bool
Database::initializeIfNeeded(const QString& path)
{
    if (m_db.isOpen())
        return true;

    if (!m_db.isValid())
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    if (!m_db.isValid())
        return setError("QSqlDatabase error");
    m_db.setDatabaseName(path);
    if (!m_db.open())
        return setError(QString("Failed to open %1: %2").arg(path).arg(m_db.lastError().text()));
    if (!isInitialized())
    {
        if (!isInitialized())
        {
            QFile file(":/dbschema.sql");
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                while (!file.atEnd())
                {
                    QByteArray line = file.readLine();
                    while (!line.endsWith(";\n") && !file.atEnd())
                        line += file.readLine();
                    if (m_db.exec(line).lastError().isValid())
                        return setError(QString("Failed to apply internal schema: %1\n%2").arg(m_db.lastError().text()).arg(QString(line)));
                }

                QSqlQuery query(m_db.exec());
                query.prepare("INSERT OR REPLACE INTO u1db_config VALUES ('replica_uid', :uuid)");
                query.bindValue(":uuid", QUuid::createUuid().toString());
                if (!query.exec())
                    return setError(QString("Failed to apply internal schema: %1\n%2").arg(m_db.lastError().text()).arg(query.lastQuery()));
                // Double-check
                if (query.boundValue(0).toString() != getReplicaUid())
                    return setError(QString("Invalid replica uid: %1").arg(query.boundValue(0).toString()));
            }
            else
                return setError(QString("Failed to read internal schema: FileError %1").arg(file.error()));
        }
    }
    return true;
}

Database::Database(QObject *parent) :
    QAbstractListModel(parent), m_path("")
{
    initializeIfNeeded();
}

/*!
    Used to implement QAbstractListModel
    Returns docId matching the given row index
    assuming all documents are ordered consistently
 */
QString
Database::getDocIdByRow(int row) const
{
    if (!m_db.isOpen())
        return QString();

    QSqlQuery query(m_db.exec());
    query.prepare("SELECT doc_id FROM document LIMIT 1 OFFSET :row");
    query.bindValue(":row", row);
    if (query.exec() && query.next())
        return query.value("doc_id").toString();
    return QString();
}

/*!
    Used to implement QAbstractListModel
    Implements the variables exposed to the Delegate in a model
    QVariant contents
    QString docId
    int index (built-in)
 */
QVariant
Database::data(const QModelIndex & index, int role) const
{
    QString docId(getDocIdByRow(index.row()));
    if (role == 0) // contents
        return getDocUnchecked(docId);
    if (role == 1) // docId
        return docId;
    return QVariant();
}

/*!
    Used to implement QAbstractListModel
    Defines \b{contents} and \b{docId} as variables exposed to the Delegate in a model
    \b{index} is supported out of the box.
 */
QHash<int, QByteArray>
Database::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(0, "contents");
    roles.insert(1, "docId");
    return roles;
}

/*!
    Used to implement QAbstractListModel
    The number of rows: the number of documents in the database.
 */
int
Database::rowCount(const QModelIndex & parent) const
{
    if (!m_db.isOpen())
        return 0;

    QSqlQuery query(m_db.exec());
    query.prepare("SELECT COUNT(*) AS count FROM document");
    if (!(query.exec() && query.next()))
        return 0;
    return query.value("count").toInt();
}

/*!
    Same functionality as getDoc() except it won't set lastError() and it
    doesn't implicitly try to initialize the underlying database.
    Use cases: model implementations, Document::getContents()
 */
QVariant
Database::getDocUnchecked(const QString& docId) const
{
    if (!m_db.isOpen())
        return QVariant();

    QSqlQuery query(m_db.exec());
    query.prepare("SELECT doc_rev, content FROM document WHERE doc_id = :docId");
    query.bindValue(":docId", docId);
    if (query.exec() && query.next())
    {
        // Convert JSON string to the Variant that QML expects
        QJsonDocument json(QJsonDocument::fromJson(query.value("content").toByteArray()));
        Q_EMIT docLoaded(docId, json.object().toVariantMap());
        return json.object().toVariantMap();
    }
    return QVariant();
}

/*!
    Returns the contents of a document by docId in a form that QML recognizes
    as a Variant object, it's identical to Document::getContents() with the
    same docId.
 */
QVariant
Database::getDoc(const QString& docId)
{
    if (!initializeIfNeeded())
        return QVariant();

    QSqlQuery query(m_db.exec());
    query.prepare("SELECT document.doc_rev, document.content, "
        "count(conflicts.doc_rev) AS conflicts FROM document LEFT OUTER JOIN "
        "conflicts ON conflicts.doc_id = document.doc_id WHERE "
        "document.doc_id = :docId GROUP BY document.doc_id, "
        "document.doc_rev, document.content");
    query.bindValue(":docId", docId);
    if (query.exec())
    {
        if (query.next())
        {
            if (query.value("conflicts").toInt() > 0)
                setError(QString("Conflicts in %1").arg(docId));
            // Convert JSON string to the Variant that QML expects
            QJsonDocument json(QJsonDocument::fromJson(query.value("content").toByteArray()));
            Q_EMIT docLoaded(docId, json.object().toVariantMap());
            return json.object().toVariantMap();
        }
        return setError(QString("Failed to get document %1: No document").arg(docId)) ? QVariant() : QVariant();
    }
    return setError(QString("Failed to get document %1: %2\n%3").arg(docId).arg(query.lastError().text()).arg(query.lastQuery())) ? QVariant() : QVariant();
}

static int
increaseVectorClockRev(int oldRev)
{
    return oldRev;
}

/*!
    Updates the existing contents of the document identified by docId if
    there's no error.
    If no docId is given or docId is an empty string the contents will be
    stored under an autogenerated name.
    Returns the new revision of the document, or -1 on failure.
 */
int
Database::putDoc(QVariant newDoc, const QString& newOrEmptyDocId)
{
    if (!initializeIfNeeded())
        return -1;

    QString docId(newOrEmptyDocId);
    QVariant oldDoc = docId.isEmpty() ? QVariant() : getDocUnchecked(docId);
    /* FIXME: Conflicts */

    int newRev = increaseVectorClockRev(7/*newDoc.rev*/);
    QSqlQuery query(m_db.exec());
    if (oldDoc.isValid())
    {
        query.prepare("UPDATE document SET doc_rev=:docRev, content=:docJson WHERE doc_id = :docId");
        query.bindValue(":docId", docId);
        query.bindValue(":docRev", newRev);
        // Parse Variant from QML as JsonDocument, fallback to string
        QString json(QJsonDocument::fromVariant(newDoc).toJson());
        query.bindValue(":docJson", json.isEmpty() ? newDoc : json);
        if (!query.exec())
            return setError(QString("Failed to put/ update document %1: %2\n%3").arg(docId).arg(query.lastError().text()).arg(query.lastQuery())) ? -1 : -1;
        query.prepare("DELETE FROM document_fields WHERE doc_id = :docId");
        query.bindValue(":docId", docId);
        if (!query.exec())
            return setError(QString("Failed to delete document field %1: %2\n%3").arg(docId).arg(query.lastError().text()).arg(query.lastQuery())) ? -1 : -1;
    }
    else
    {
        if (docId.isEmpty())
            docId = QString("D-%1").arg(QUuid::createUuid().toString().mid(1).replace("}",""));
        if (!QRegExp("^[a-zA-Z0-9.%_-]+$").exactMatch(docId))
            return setError(QString("Invalid docID %1").arg(docId)) ? -1 : -1;

        query.prepare("INSERT INTO document (doc_id, doc_rev, content) VALUES (:docId, :docRev, :docJson)");
        query.bindValue(":docId", docId);
        query.bindValue(":docRev", newRev);
        // Parse Variant from QML as JsonDocument, fallback to string
        QJsonDocument json(QJsonDocument::fromVariant(newDoc));
        query.bindValue(":docJson", json.isEmpty() ? newDoc : json.toJson());
        if (!query.exec())
            return setError(QString("Failed to put document %1: %2\n%3").arg(docId).arg(query.lastError().text()).arg(query.lastQuery())) ? -1 : -1;
    }

    beginResetModel();
    endResetModel();
    /* FIXME investigate correctly notifying about new rows
    beginInsertRows(QModelIndex(), rowCount(), 0);
    endInsertRows();
    */
    Q_EMIT docChanged(docId, newDoc);

    return newRev;
}

QList<QString>
Database::listDocs()
{
    QList<QString> list;
    if (!initializeIfNeeded())
        return list;

    QSqlQuery query(m_db.exec());
    query.prepare("SELECT document.doc_id, document.doc_rev, document.content, "
        "count(conflicts.doc_rev) FROM document LEFT OUTER JOIN conflicts "
        "ON conflicts.doc_id = document.doc_id GROUP BY document.doc_id, "
        "document.doc_rev, document.content");
    if (query.exec())
    {
        while (query.next())
        {
            list.append(query.value("doc_id").toString());
        }
        return list;
    }
    return setError(QString("Failed to list documents: %1\n%2").arg(query.lastError().text()).arg(query.lastQuery())) ? list : list;
}

/*!
    A relative filename or absolute path advises the database to store documents
    and indexes persistently on disk. Internally, an SQlite database is written.

    If no path is set, as is the default, all database contents are written in
    memory only. The same affect can be achieved by passing the string ":memory:".
 */
void
Database::setPath(const QString& path)
{
    if (m_path == path)
        return;

    beginResetModel();
    m_db.close();
    // TODO: relative path
    initializeIfNeeded(path);
    endResetModel();

    m_path = path;
    Q_EMIT pathChanged(path);
}

/*!
   The persistent storage location if set. By default the database is only
   storted in memory. See setPath().
 */
QString
Database::getPath()
{
    return m_path;
}

/*!
   Stores a new index under the given name. An existing index won't be
   replaced implicitly, an error will be set in that case.
 */
QString
Database::putIndex(const QString& indexName, QStringList expressions)
{
    if (indexName.isEmpty() || expressions.isEmpty())
        return QString("Either name or expressions is empty");

    Q_FOREACH (QString expression, expressions)
        if (expression.isEmpty() || expression.isNull())
            return QString("Empty expression in list");

    if (!initializeIfNeeded())
        return QString("Database isn't ready");

    QStringList results = getIndexExpressions(indexName);
    bool changed = false;
    Q_FOREACH (QString expression, expressions)
        if (results.contains(expression))
            changed = true;
    if (changed)
        return QString("Index conflicts with existing index");

    QSqlQuery query(m_db.exec());
    for (int i = 0; i < expressions.count(); ++i)
    {
        query.prepare("INSERT INTO index_definitions VALUES (:indexName, :offset, :field)");
        query.bindValue(":indexName", indexName);
        query.bindValue(":offset", i);
        query.bindValue(":field", expressions.at(i));
        if (!query.exec())
            return QString("Failed to insert index definition: %1\n%2").arg(m_db.lastError().text()).arg(query.lastQuery());
    }
    return QString();
}

/*!
   Gets the expressions saved with putIndex().
 */
QStringList
Database::getIndexExpressions(const QString& indexName)
{
    QStringList expressions;

    if (!initializeIfNeeded())
        return expressions;

    QSqlQuery query(m_db.exec());
    query.prepare("SELECT field FROM index_definitions WHERE name = :indexName ORDER BY offset DESC");
    query.bindValue(":indexName", indexName);
    if (!query.exec())
        return setError(QString("Failed to lookup index definition: %1\n%2").arg(m_db.lastError().text()).arg(query.lastQuery())) ? expressions : expressions;

    while (query.next())
         expressions.append(query.value("field").toString());
    return expressions;
}

/*!
   Lists the index keys of an index created with putIndex().
 */
QStringList
Database::getIndexKeys(const QString& indexName)
{
    QStringList list;
    if (!initializeIfNeeded())
        return list;

    QStringList expressions = getIndexExpressions(indexName);
    QString valueFields, tables, noValueWhere;
    int i = 0;
    Q_FOREACH (QString expression, expressions)
    {
        valueFields += QString("d%1.value,").arg(i);
        tables += QString("document_fields d%1,").arg(i);
        noValueWhere += QString("d.doc_id = d%1.doc_id AND d%1.field_name = \"%2\" AND ").arg(
            i).arg(expression);
    }
    if (valueFields.endsWith(","))
        valueFields.chop(1);
    if (tables.endsWith(","))
        tables.chop(1);
    if (noValueWhere.endsWith("AND "))
        noValueWhere.chop(4);

    QString where;
    i = 0;
    Q_FOREACH (QString expression, expressions)
    {
        where += QString("%1 AND d%2.value NOT NULL AND ").arg(noValueWhere).arg(i);
        i++;
    }
    if (where.endsWith("AND "))
        where.chop(4);

    QSqlQuery query(m_db.exec());
    query.prepare(QString("SELECT %1 FROM document d, %2 WHERE %3 GROUP BY %1").arg(
        valueFields, tables, where));
    if (!query.exec())
        return setError(QString("Failed to get index keys: %1\n%2").arg(m_db.lastError().text()).arg(query.lastQuery())) ? list : list;

    while (query.next())
        list.append(query.value("value").toString());
    return list;
}

QT_END_NAMESPACE_U1DB

#include "moc_database.cpp"

