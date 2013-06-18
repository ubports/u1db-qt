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
    \inmodule U1Db
    \ingroup modules

    \brief The Database class implements the on-disk storage of an individual
    U1DB database.

    Database can be used as a QAbstractListModel, delegates will then have access to \a docId and \a contents
    analogous to the properties of Document.
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
    \property Database::error
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

    if (!m_db.isValid())
        m_db = QSqlDatabase::addDatabase("QSQLITE",QUuid::createUuid().toString());
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
    Returns the contents of a document by \a docId in a form that QML recognizes
    as a Variant object, it's identical to Document::getContents() with the
    same \a docId.
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

QString Database::getNextDocRevisionNumber(QString doc_id)
{

    QString revision_number = getReplicaUid()+":1";

    QString current_revision_number = getCurrentDocRevisionNumber(doc_id);

    QStringList current_revision_list = current_revision_number.split("|");

    Q_FOREACH (QString current_revision, current_revision_list) {

        QStringList current_revision_number_list = current_revision.split(":");

        if(current_revision_number_list[0]==getReplicaUid()) {

            int revision_generation_number = current_revision_number_list[1].toInt()+1;

            revision_number = getReplicaUid()+":"+QString::number(revision_generation_number);

        }
        else {
            revision_number+="|"+current_revision;
        }

    }

    revision_number = revision_number.replace("{","");

    revision_number = revision_number.replace("}","");

    return revision_number;

}

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
    return QString();
}

int Database::getCurrentGenerationNumber(){

    int sequence_number = -1;

    QSqlQuery query(m_db.exec());

    qDebug()<<"Select";

    query.prepare("SELECT seq FROM sqlite_sequence WHERE name = 'transaction_log'");

    qDebug()<<"Select";

    if (query.exec())
    {
        while (query.next())
        {
            sequence_number = (query.value("seq").toInt());

        }

    }

    return sequence_number;

}

int Database::updateCurrentGenerationNumber(){

    int sequence_number = getCurrentGenerationNumber();

    QSqlQuery query(m_db.exec());


    if(sequence_number != -1){

        qDebug()<<"Update";

        query.prepare("UPDATE sqlite_sequence SET seq = :new_seq WHERE name = 'transaction_log'");

        qDebug()<<"Update";

        query.bindValue(":new_seq", (sequence_number+1));

        if (!query.exec())
            return -1;
    }
    else{

        sequence_number = 1;

        qDebug()<<"Insert";

        query.prepare("INSERT INTO sqlite_sequence(name,seq) values('transaction_log',1)");

        qDebug()<<"Insert";

        if (!query.exec())
            return -1;

    }

    return sequence_number;

}



QString Database::generateNewTransactionId(){

    QString uid = "T-"+QUuid::createUuid().toString();
    uid = uid.replace("}","");
    uid = uid.replace("{","");
    return uid;

}

int Database::createNewTransaction(QString doc_id){

     //generate new transaction here -- update transaction_log & sqlite_sequence

    int generation = getCurrentGenerationNumber();

    if(generation !=-1){

        QString transaction_id = generateNewTransactionId();

        QSqlQuery query(m_db.exec());

        query.prepare("INSERT INTO transaction_log VALUES(:generation,:doc_id,:transaction_id");
        query.bindValue(":doc_id", doc_id);
        query.bindValue(":generation", generation);
        query.bindValue(":transaction_id", transaction_id);

        if (!query.exec())
            return -1;
        else
            return 0;

    }

    return -1;
}

/*!
    Updates the existing \a contents of the document identified by \a docId if
    there's no error.
    If no \a docId is given or \a docId is an empty string the \a contents will be
    stored under an autogenerated name.
    Returns the new revision of the document, or -1 on failure.
 */
int
Database::putDoc(QVariant contents, const QString& docId)
{
    if (!initializeIfNeeded())
        return -1;

    QString newOrEmptyDocId(docId);
    QVariant oldDoc = newOrEmptyDocId.isEmpty() ? QVariant() : getDocUnchecked(newOrEmptyDocId);
    /* FIXME: Conflicts */

    int newRev = increaseVectorClockRev(7/*contents.rev*/); // maybe this can be removed as it is replaced by revision_number.

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
            return setError(QString("Failed to put/ update document %1: %2\n%3").arg(newOrEmptyDocId).arg(query.lastError().text()).arg(query.lastQuery())) ? -1 : -1;
        query.prepare("DELETE FROM document_fields WHERE doc_id = :docId");
        query.bindValue(":docId", newOrEmptyDocId);
        if (!query.exec())
            return setError(QString("Failed to delete document field %1: %2\n%3").arg(newOrEmptyDocId).arg(query.lastError().text()).arg(query.lastQuery())) ? -1 : -1;

        createNewTransaction(newOrEmptyDocId);

    }
    else
    {
        if (newOrEmptyDocId.isEmpty())
            newOrEmptyDocId = QString("D-%1").arg(QUuid::createUuid().toString().mid(1).replace("}",""));
        if (!QRegExp("^[a-zA-Z0-9.%_-]+$").exactMatch(newOrEmptyDocId))
            return setError(QString("Invalid docID %1").arg(newOrEmptyDocId)) ? -1 : -1;

        query.prepare("INSERT INTO document (doc_id, doc_rev, content) VALUES (:docId, :docRev, :docJson)");
        query.bindValue(":docId", newOrEmptyDocId);
        query.bindValue(":docRev", revision_number);
        // Parse Variant from QML as JsonDocument, fallback to string
        QJsonDocument json(QJsonDocument::fromVariant(contents));
        query.bindValue(":docJson", json.isEmpty() ? contents : json.toJson());
        if (!query.exec())
            return setError(QString("Failed to put document %1: %2\n%3").arg(docId).arg(query.lastError().text()).arg(query.lastQuery())) ? -1 : -1;

        createNewTransaction(newOrEmptyDocId);
    }

    beginResetModel();
    endResetModel();
    /* FIXME investigate correctly notifying about new rows
    beginInsertRows(QModelIndex(), rowCount(), 0);
    endInsertRows();
    */

    Q_EMIT docChanged(newOrEmptyDocId, contents);

    return newRev;
}

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
    \property Database::path
    A relative filename or absolute path to store documents
    and indexes persistently on disk. By default documents are stored in memory.
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

QT_END_NAMESPACE_U1DB

#include "moc_database.cpp"

