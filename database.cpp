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

#include "database.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

QString
Database::getReplicaUid()
{
    QSqlQuery query (m_db.exec("SELECT value FROM u1db_config WHERE name = 'replica_uid'"));
    if (!query.lastError().isValid() && query.next())
        return query.value(0).toString();
    return setError(QString("Failed to get replica UID: %1\n%2").arg(query.lastError().text()).arg(query.lastQuery())) ? QString() : QString();
}

bool
Database::isInitialized()
{
    m_db.exec("PRAGMA case_sensitive_like=ON");
    QSqlQuery query(m_db.exec(
        "SELECT value FROM u1db_config WHERE name = 'sql_schema'"));
    return query.next();
}

bool
Database::setError(const QString& error)
{
    qDebug() << "u1db: " << error;
    m_error = error;
    Q_EMIT errorChanged(error);
    return false;
}

QString
Database::lastError()
{
    return m_error;
}

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
        m_db.exec("BEGIN EXCLUSIVE");
        if (!isInitialized())
        {
            // QFile file("qrc:///dbschema.sql");
            QFile file("../dbschema.sql");
            if (!file.exists ())
                file.setFileName("./dbschema.sql");
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

QVariant
Database::data(const QModelIndex & index, int role) const
{
    qDebug() << "::data";
    return QVariant();
}

int
Database::rowCount(const QModelIndex & parent) const
{
    qDebug() << "::rowCount";
    if (!m_db.isOpen())
        return -1;

    QSqlQuery query(m_db.exec());
    query.prepare("SELECT COUNT(*) AS count FROM document");
    if (!(query.exec() && query.next()))
        return -1;
    qDebug() << "rowCount" << query.value("count").toInt();
    return query.value("count").toInt();
}

QVariant
Database::getDoc(const QString& docId, bool checkConflicts, bool fallbackToEmpty)
{
    if (!initializeIfNeeded())
        return QVariant();

    QSqlQuery query(m_db.exec());
    if (checkConflicts)
        query.prepare("SELECT document.doc_rev, document.content, "
            "count(conflicts.doc_rev) FROM document LEFT OUTER JOIN "
            "conflicts ON conflicts.doc_id = document.doc_id WHERE "
            "document.doc_id = :docId GROUP BY document.doc_id, "
            "document.doc_rev, document.content");
    else
        query.prepare("SELECT doc_rev, content, 0 FROM document WHERE doc_id = :docId");
    query.bindValue(":docId", docId);
    if (query.exec())
    {
        if (query.next())
            return query.value("content");
        if (fallbackToEmpty)
            return QVariant();
        return setError(QString("Failed to get document %1: No document").arg(docId)) ? QVariant() : QVariant();
    }
    return setError(QString("Failed to get document %1: %2\n%3").arg(docId).arg(query.lastError().text()).arg(query.lastQuery())) ? QVariant() : QVariant();
}

static int
increaseVectorClockRev(int oldRev)
{
    return oldRev;
}

int
Database::putDoc(QVariant newDoc, const QString& docId)
{
    if (!initializeIfNeeded())
        return -1;

    QVariant oldDoc = getDoc(docId, true, true);
    /* TODO: if (oldDoc.isValid() && oldDoc.has_conflicts)
        return setError(QString("Conflicts in %1").arg(docId)) ? -1 : -1; */

    int newRev = increaseVectorClockRev(7/*newDoc.rev*/);
    QSqlQuery query(m_db.exec());
    if (oldDoc.isValid())
    {
        query.prepare("UPDATE document SET doc_rev=:docRev, content=:docJson WHERE doc_id = :docId");
        query.bindValue(":docId", docId);
        query.bindValue(":docRev", newRev);
        query.bindValue(":docJson", QJsonDocument::fromVariant(newDoc).toJson());
        if (!query.exec())
            return setError(QString("Failed to put/ update document %1: %2\n%3").arg(docId).arg(query.lastError().text()).arg(query.lastQuery())) ? -1 : -1;
        query.prepare("DELETE FROM document_fields WHERE doc_id = :docId");
        query.bindValue(":docId", docId);
        if (!query.exec())
            return setError(QString("Failed to delete document field %1: %2\n%3").arg(docId).arg(query.lastError().text()).arg(query.lastQuery())) ? -1 : -1;
    }
    else
    {
        QString newDocId(docId);
        if (newDocId.isEmpty())
            newDocId = QString("D-%1").arg(QUuid::createUuid().toString().mid(1).replace("}",""));
        if (!QRegExp("^[a-zA-Z0-9.%_-]+$").exactMatch(newDocId))
            return setError(QString("Invalid docID %1").arg(newDocId)) ? -1 : -1;

        query.prepare("INSERT INTO document (doc_id, doc_rev, content) VALUES (:docId, :docRev, :docJson)");
        query.bindValue(":docId", newDocId);
        query.bindValue(":docRev", newRev);
        query.bindValue(":docJson", QJsonDocument::fromVariant(newDoc).toJson());
        if (!query.exec())
            return setError(QString("Failed to put document %1: %2\n%3").arg(docId).arg(query.lastError().text()).arg(query.lastQuery())) ? -1 : -1;
    }
    return newRev;
}

QList<QVariant>
Database::listDocs()
{
    initializeIfNeeded();

    QSqlQuery query(m_db.exec());
    query.prepare("SELECT document.doc_id, document.doc_rev, document.content, "
        "count(conflicts.doc_rev) FROM document LEFT OUTER JOIN conflicts "
        "ON conflicts.doc_id = document.doc_id GROUP BY document.doc_id, "
        "document.doc_rev, document.content");
    if (query.exec())
    {
        QList<QVariant> list;
        while (query.next())
        {
            QVariant newDoc(query.value("content"));
            list.append(newDoc);
        }
        return list;
    }
    return setError(QString("Failed to list documents: %1\n%2").arg(query.lastError().text()).arg(query.lastQuery())) ? QList<QVariant>() : QList<QVariant>();
}

void
Database::setPath(const QString& path)
{
    if (m_path == path)
        return;

    m_db.close();
    // TODO: relative path
    initializeIfNeeded(path);

    m_path = path;
    Q_EMIT pathChanged(path);
}

QString
Database::getPath()
{
    return m_path;
}

/* int Database::createIndexList(QString index_name, QStringList expressions) 

corresponds to the u1db.c function:

int u1db_create_index_list(u1database *db, const char *index_name, int n_expressions, const char **expressions) 

It is currently marked INCOMPLETE */


/* Tests to create for createIndexList: does it return a value, what is the value it returns, is the value returned an acceptable value, what is the expected result for an index_name that we know is unique as well as one we know is not unique, is the database NULL or not, is the expressions list empty or not, is the index_name empty or not
*/

int Database::createIndexList(QString index_name, QStringList expressions)
{
    int U1DB_OK, U1DB_INVALID_PARAMETER, U1DB_DUPLICATE_INDEX_NAME, SQLITE_OK = 0; // These are here temporarily
    int status = U1DB_OK; 
    int i = 0;

    if (m_db.isOpen() == false  || index_name.isEmpty() == false || expressions.isEmpty() == false) {
        return U1DB_INVALID_PARAMETER; // is this really the correct return value for all three of the conditions above?
    }
    
    for (i = 0; i < expressions.count(); ++i) {
        if (expressions[i].isEmpty() == true || expressions[i].isNull() == true) {
            return U1DB_INVALID_PARAMETER;
        }
    }
       
    //status = u1db__find_unique_expressions(db, n_expressions, expressions, &n_unique, &unique_expressions); // needs to be modified

    if (status != U1DB_OK) {
        return status;
    }
        
    //QString statement = "SELECT field FROM index_definitions WHERE name = ".index_name." ORDER BY offset DESC";
    QSqlQuery query; 
    query.prepare("SELECT field FROM index_definitions WHERE name = ? ORDER BY offset DESC");
    query.addBindValue(index_name);
    
    QStringList results;
    
    //QSqlQuery query(statement);
    
    // Add a check to ensure our query was OK -- what return value do we need here if there was an error rather than simply an empty set of results?
    
    while (query.next()) {    	
         results.append(query.value(0).toString());  
    }
    
    for (i = 0; i < expressions.count(); i++) {
        if (results.contains(expressions.at(i))==true) {
            return U1DB_DUPLICATE_INDEX_NAME;
        }
    }     
        
    if (results.count() > 0) {
        status = SQLITE_OK;
        return status;
    }
    else{
    	
    
    // Should probably use a prepared statement here ... or some alternative
    
    for (i = 0; i < expressions.count(); ++i) {
    	   							
     			query.prepare("INSERT INTO index_definitions VALUES (?, ?, ?)");
     			query.addBindValue(index_name);
     			query.addBindValue(i);
     			query.addBindValue(expressions.at(i));
     			
     			if(query.exec()==false){
     				// Actually in the orginal C function the check was to see if the return value was SQLITE_CONSTRAINT and not only whether the exec generally failed
     				status = U1DB_DUPLICATE_INDEX_NAME;
     				// Is it safe to assume that all insert statements in this loop need to be rolled back?
     				return status;
     			}
     			
    }
    
    //status = u1db__index_all_docs(db, n_unique, unique_expressions); // Needs to be modified
    
    return status;
    
    }
    
    
    
}


/* int Database::createIndex(QString index_name, QStringList expressions)

corresponds to the u1db.c function:

int u1db_create_index(u1database *db, const char *index_name, int n_expressions, ...) 

It is being marked as INCOMPLETE */

int Database::createIndex(QString index_name, QStringList expressions)
{
   int U1DB_OK, U1DB_NOMEM;
   int i;

   int status = U1DB_OK;
        
   if(expressions.isEmpty() == true) {
	status = U1DB_NOMEM;	
   }
   else
   {    		
     status = createIndexList(index_name, expressions);
     return status;
   }
    
}

QT_END_NAMESPACE_U1DB

#include "moc_database.cpp"

