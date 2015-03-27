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

#include <QSqlQuery>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include <QSqlError>
#include <QUuid>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>

#include "database.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

namespace
{
class ScopedTransaction
{
public:
    ScopedTransaction(QSqlDatabase &db) :
            m_db(db),
            m_transaction(false)
    {
        m_transaction = m_db.transaction();
    }

    ~ScopedTransaction()
    {
        if (m_transaction)
        {
            m_db.commit();
        }
    }

    QSqlDatabase &m_db;

    bool m_transaction;
};
}

/*!
    \class Database
    \inmodule U1Db
    \ingroup cpp

    \brief The Database class implements on-disk storage for documents and indexes.

    Database can be used as a QAbstractListModel, delegates will then have access to \a docId and \a contents
    analogous to the properties of Document.
*/

/*!
    \qmltype Database
    \instantiates Database
    \inqmlmodule U1Db 1.0
    \ingroup modules

    \brief Database implements on-disk storage for documents and indexes.

    In a ListView the Database can be used as a model which includes all documents
    in the database. For listing only a subset of documents Query can be used.

    \qml
    ListView {
        model: Database {
            id: myDatabase
        }
        delegate: ListItem.Subtitled {
            text: docId
            subText: contents.color
        }
    }
    \endqml

    \sa Query
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
    qWarning("u1db: %s", qPrintable(error));
    m_error = error;
    Q_EMIT errorChanged(error);
    return false;
}

/*!
    \qmlproperty string Database::error
    The last error as a string if the last operation failed.
 */
/*!
    The last error as a string if the last operation failed.
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

    /* A unique ID is used for the connection name to ensure that we aren't
       re-using or replacing other opend databases. */
    if (!m_db.isValid())
        m_db = QSqlDatabase::addDatabase("QSQLITE",QUuid::createUuid().toString());

    if (!m_db.isValid())
        return setError("QSqlDatabase error");

    if (path != ":memory:" && QDir::isRelativePath(path)) {
        QString dataPath(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
        QString absolutePath(QDir(dataPath).absoluteFilePath(path));
        QString parent(QFileInfo(absolutePath).dir().path());
        if (!QDir().mkpath(parent))
            setError(QString("Failed to make data folder %1").arg(parent));
        m_db.setDatabaseName(absolutePath);
    }
    else
    {
        QDir parent(QFileInfo(path).dir());
        if (!parent.mkpath(parent.path()))
            setError(QString("Failed to make parent folder %1").arg(parent.path()));
        m_db.setDatabaseName(path);
    }

    if (!m_db.open())
        return setError(QString("Failed to open %1: %2").arg(path).arg(m_db.lastError().text()));
    if (!isInitialized())
    {
        if (!isInitialized())
        {
            QFile file(":/dbschema.sql");
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                ScopedTransaction t(m_db);

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

/*!
    Instantiate a new Database with an optional \a parent,
    usually by declaring it as a QML item.
 */
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
    \internal
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
    \internal
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
    \internal
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
    Same functionality as Database::getDoc() except it won't set Database::lastError() and it
    doesn't implicitly try to initialize the underlying database.
    \a docId must be a valid unique ID string
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
 * \internal
 * \brief Database::getDocumentContents
 *
 * Returns the string representation of a document that has
 * been selected from the database using a document id.
 *
 */

QString
Database::getDocumentContents(const QString& docId)
{
    if (!initializeIfNeeded())
        return QString();

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
            return query.value("content").toString();
        }
        return setError(QString("Failed to get document %1: No document").arg(docId)) ? QString() : QString();
    }
    return setError(QString("Failed to get document %1: %2\n%3").arg(docId).arg(query.lastError().text()).arg(query.lastQuery())) ? QString() : QString();
}


/*!
    \qmlmethod Variant Database::getDoc(string)
    Returns the contents of a document by \a docId in a form that QML recognizes
    as a Variant object, it's identical to Document::getContents() with the
    same \a docId.
 */
/*!
 *  Returns the contents of a document by \a docId in a form that QML recognizes
 *  as a Variant object, it's identical to Document::getContents() with the
 *  same \a docId.
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

/*!
 * \internal
  This function creates a new revision number.

  It returns a string for use in the document table's 'doc_rev' field.
 */

QString Database::getNextDocRevisionNumber(QString doc_id)
{

    QString revision_number = getReplicaUid()+":1";

    QString current_revision_number = getCurrentDocRevisionNumber(doc_id);

    /*!
        Some revisions contain information from previous
conflicts/syncs. Revisions are delimited by '|'.

     */

    QStringList current_revision_list = current_revision_number.split("|");

    Q_FOREACH (QString current_revision, current_revision_list) {

        /*!
            Each revision contains two pieces of information,
the uid of the database that made the revsion, and a counter
for the revsion. This information is delimited by ':'.

         */

        QStringList current_revision_number_list = current_revision.split(":");

        if(current_revision_number_list[0]==getReplicaUid()) {

            /*!
                If the current revision uid  is the same as this Database's uid the counter portion is increased by one.

             */

            int revision_generation_number = current_revision_number_list[1].toInt()+1;

            revision_number = getReplicaUid()+":"+QString::number(revision_generation_number);

        }
        else {

            /*!
                If the current revision uid  is not the same as this Database's uid then the revision represents a change that originated in another database.

             */
            //revision_number+="|"+current_revision;

            /* Not sure if the above is necessary,
             *and did not appear to be working as intended either.
             *
             * Commented out, but maybe OK to delete.
             */
        }

    }

    /*!
        The Database UID has curly brackets, but they are not required for the revision number and need to be removed.

     */

    revision_number = revision_number.replace("{","");

    revision_number = revision_number.replace("}","");

    return revision_number;

}

/*!
 * \internal
    The getCurrentDocRevisionNumber(QString doc_id) function
returns the current string value from the document table's
doc_rev field.

 */


QString Database::getCurrentDocRevisionNumber(QString doc_id){
    if (!initializeIfNeeded())
        return QString();

    QSqlQuery query(m_db.exec());

    query.prepare("SELECT doc_rev from document WHERE doc_id = :docId");
    query.bindValue(":docId", doc_id);

    if (query.exec())
    {
        while (query.next())
        {
            return query.value("doc_rev").toString();
        }

    }
    else{
        return setError(query.lastError().text()) ? QString() : QString();
    }
    return QString();
}

/*!
 * \internal
 * \brief Database::updateSyncLog
 *
 * This method is used at the end of a synchronization session,
 * to update the database with the latest information known about the peer
 * database that was synced against.
 */
void Database::updateSyncLog(bool insert, QString uid, QString generation, QString transaction_id)
{

    if (!initializeIfNeeded())
        return;

    QSqlQuery query(m_db.exec());

    if(insert==true){
        query.prepare("INSERT INTO sync_log(known_generation,known_transation_id,known_transation_id) VALUES(:knownGeneration, :knownTransactionId, :replicaUid)");

    }
    else{
        query.prepare("UPDATE sync_log SET known_generation = :knownGeneration, known_transation_id = :knownTransactionId WHERE replica_uid = :replicaUid");
    }


    query.bindValue(":replicaUid", uid);
    query.bindValue(":knownGeneration", generation);
    query.bindValue(":knownTransactionId", transaction_id);
    if (!query.exec())
    {
        setError(query.lastError().text());

    }

}

/*!
 * \internal
 *
 * Whenever a document as added or modified it needs a new revision number.
 *
 * The revision number contains information about revisions made at the source,
 * but also revisions to the document by target databases (and then synced with the source).
 *
 */

void Database::updateDocRevisionNumber(QString doc_id,QString revision){
    if (!initializeIfNeeded())
        return;

    QSqlQuery query(m_db.exec());

    query.prepare("UPDATE document SET doc_rev = :revisionId WHERE doc_id = :docId");
    query.bindValue(":docId", doc_id);
    query.bindValue(":revisionId", revision);
    if (!query.exec())
    {
        setError(query.lastError().text());

    }

}

/*!
    The getCurrentGenerationNumber() function searches for the
current generation number from the  sqlite_sequence table.
The return value can then be used during a synchronization session,
amongst other things.

 */

int Database::getCurrentGenerationNumber(){

    int sequence_number = -1;

    QSqlQuery query(m_db.exec());

    query.prepare("SELECT seq FROM sqlite_sequence WHERE name = 'transaction_log'");

    if (query.exec())
    {
        while (query.next())
        {
            sequence_number = (query.value("seq").toInt());

        }

    }
    else{
        setError(query.lastError().text());
    }

    return sequence_number;

}

/*!
    The generateNewTransactionId() function generates a random
transaction id string, for use when creating new transations.

 */

QString Database::generateNewTransactionId(){

    QString uid = "T-"+QUuid::createUuid().toString();
    uid = uid.replace("}","");
    uid = uid.replace("{","");
    return uid;

}

/*!
    Each time a document in the Database is created or updated a
new transaction is performed, and information about it inserted into the
transation_log table using the createNewTransaction(QString doc_id)
function.

 */

int Database::createNewTransaction(QString doc_id){

    QString transaction_id = generateNewTransactionId();

    QSqlQuery query(m_db.exec());

    QString queryString = "INSERT INTO transaction_log(doc_id, transaction_id) VALUES('"+doc_id+"', '"+transaction_id+"')";

    if (!query.exec(queryString)){
        return -1;
    }
    else{
        return 0;
    }

    return -1;
}

/*!
    \qmlmethod string Database::putDoc(var, string)
    Updates the existing \a contents of the document identified by \a docId if
    there's no error.
    If no \a docId is given or \a docId is an empty string the \a contents will be
    stored under an autogenerated name.
    Returns the new revision of the document, or -1 on failure.
 */
/*!
    Updates the existing \a contents of the document identified by \a docId if
    there's no error.
    If no \a docId is given or \a docId is an empty string the \a contents will be
    stored under an autogenerated name.
    Returns the new revision of the document, or -1 on failure.
 */
QString
Database::putDoc(QVariant contents, const QString& docId)
{
    if (!initializeIfNeeded())
        return "";

    if (contents.canConvert<QVariantMap>())
        contents = contents.value<QVariantMap>();

    ScopedTransaction t(m_db);

    QString newOrEmptyDocId(docId);
    QVariant oldDoc = newOrEmptyDocId.isEmpty() ? QVariant() : getDocUnchecked(newOrEmptyDocId);

    QString revision_number = getNextDocRevisionNumber(newOrEmptyDocId);

    QSqlQuery query(m_db.exec());
    if (oldDoc.isValid())
    {
        query.prepare("UPDATE document SET doc_rev=:docRev, content=:docJson WHERE doc_id = :docId");
        query.bindValue(":docId", newOrEmptyDocId);
        query.bindValue(":docRev", revision_number);
        // Parse Variant from QML as JsonDocument, fallback to string
        QString json(QJsonDocument::fromVariant(contents).toJson());
        query.bindValue(":docJson", json.isEmpty() ? contents : json);
        if (!query.exec())
            return setError(QString("Failed to put/ update document %1: %2\n%3").arg(newOrEmptyDocId).arg(query.lastError().text()).arg(query.lastQuery())) ? "" : "";
        query.prepare("DELETE FROM document_fields WHERE doc_id = :docId");
        query.bindValue(":docId", newOrEmptyDocId);
        if (!query.exec())
            return setError(QString("Failed to delete document field %1: %2\n%3").arg(newOrEmptyDocId).arg(query.lastError().text()).arg(query.lastQuery())) ? "" : "";

        createNewTransaction(newOrEmptyDocId);

    }
    else
    {
        if (newOrEmptyDocId.isEmpty())
            newOrEmptyDocId = QString("D-%1").arg(QUuid::createUuid().toString().mid(1).replace("}",""));
        if (!QRegExp("^[a-zA-Z0-9.%_-]+$").exactMatch(newOrEmptyDocId))
            return setError(QString("Invalid docID %1").arg(newOrEmptyDocId)) ? "" : "";

        query.prepare("INSERT INTO document (doc_id, doc_rev, content) VALUES (:docId, :docRev, :docJson)");
        query.bindValue(":docId", newOrEmptyDocId);
        query.bindValue(":docRev", revision_number);
        // Parse Variant from QML as JsonDocument, fallback to string
        QJsonDocument json(QJsonDocument::fromVariant(contents));
        query.bindValue(":docJson", json.isEmpty() ? contents : json.toJson());
        if (!query.exec())
            return setError(QString("Failed to put document %1: %2\n%3").arg(docId).arg(query.lastError().text()).arg(query.lastQuery())) ? "" : "";

        createNewTransaction(newOrEmptyDocId);
    }

    beginResetModel();
    endResetModel();
    /* FIXME investigate correctly notifying about new rows
    beginInsertRows(QModelIndex(), rowCount(), 0);
    endInsertRows();
    */

    Q_EMIT docChanged(newOrEmptyDocId, contents);

    return revision_number;
}

/*!
    \qmlmethod void Database::deleteDoc(string)
    Deletes the document identified by \a docId.
 */
/*!
    Deletes the document identified by \a docId.
 */
void
Database::deleteDoc(const QString& docId)
{
    putDoc(QString(), docId);
}

/*!
 * \brief Database::resetModel
 *
 * Resets the Database model.
 */

void Database::resetModel(){

    beginResetModel();
    endResetModel();

}


/*!
    \qmlmethod list<string> Database::listDocs()
    Returns a list of all stored documents by their docId.
 */
/*!
    Returns a list of all stored documents by their docId.
 */
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
    \qmlproperty string Database::path
    A relative \a path can be given to store the database in an app-specific
    writable folder. This is recommended as it ensures to work with confinement.
    If more control is needed absolute paths or local file URIs can be used.
    By default or if the path is empty everything is stored in memory.
 */
/*!
    A relative \a path can be given to store the database in an app-specific
    writable folder. This is recommended as it ensures to work with confinement.
    If more control is needed absolute paths or local file URIs can be used.
    By default or if the path is empty everything is stored in memory.
 */
void
Database::setPath(const QString& path)
{
    if (m_path == path)
        return;

    beginResetModel();
    m_db.close();
    initializeIfNeeded(path);
    endResetModel();

    m_path = path;
    Q_EMIT pathChanged(path);
}

/*!
 * Returns the path of the database.
 */
QString
Database::getPath()
{
    return m_path;
}

/*!
   Stores a new index under the given \a indexName, with \a expressions.
   An existing index won't be replaced implicitly, an error will be set in that case.
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

    ScopedTransaction t(m_db);

    QStringList results = getIndexExpressions(indexName);
    bool changed = false;
    Q_FOREACH (QString expression, expressions)
        if (results.contains(expression))
            changed = true;
    if (changed)
        return QString("Index conflicts with existing index");

    QSqlQuery query(m_db.exec());
    query.prepare("INSERT INTO index_definitions VALUES (:indexName, :offset, :field)");

    QVariantList indexNameData;
    QVariantList offsetData;
    QVariantList fieldData;
    for (int i = 0; i < expressions.count(); ++i)
    {
        indexNameData << indexName;
        offsetData << i;
        fieldData << expressions.at(i);
    }
    query.addBindValue(indexNameData);
    query.addBindValue(offsetData);
    query.addBindValue(fieldData);

    if (!query.execBatch())
        return QString("Failed to insert index definition: %1\n%2").arg(m_db.lastError().text()).arg(query.lastQuery());

    return QString();
}

/*!
   Gets the expressions saved with putIndex().
   \a indexName: the unique name of an existing index
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
   \a indexName: the unique name of an existing index
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

/* Handy functions for synchronization. */

/*!
 * \internal
 * \brief Database::listTransactionsSince
 *
 * This lists transactions for the database since a particular generation number.
 *
 */

QList<QString> Database::listTransactionsSince(int generation){

    QList<QString> list;

    if (!initializeIfNeeded())
        return list;

    QSqlQuery query(m_db.exec());

    QString queryStmt = "SELECT generation, doc_id, transaction_id FROM transaction_log where generation > "+QString::number(generation);

    if (query.exec(queryStmt))
    {
        while (query.next())
        {
            list.append(query.value("generation").toString()+"|"+query.value("doc_id").toString()+"|"+query.value("transaction_id").toString());
        }

        return list;

    }

    return list;

}

/*!
 * \internal
 * \brief Database::getSyncLogInfo
 *
 * Provides the information about previous synchronizations between the database and another (if any).
 *
 */

QMap<QString,QVariant> Database::getSyncLogInfo(QMap<QString,QVariant> lastSyncInformation, QString uid, QString prefix){

    if (!initializeIfNeeded())
        return lastSyncInformation;

    QString queryStmt = "SELECT known_transaction_id, known_generation FROM sync_log WHERE replica_uid = '"+uid +"'";

    QSqlQuery query(m_db.exec());

    if (query.exec(queryStmt))
    {
        while (query.next())
        {
            lastSyncInformation.insert(prefix + "_replica_generation", query.value(1).toInt());
            lastSyncInformation.insert(prefix + "_replica_transaction_id",query.value(0).toString());
            return lastSyncInformation;
        }

    }
    else{
        setError(query.lastError().text());
    }

    return lastSyncInformation;
}

QT_END_NAMESPACE_U1DB

#include "moc_database.cpp"

