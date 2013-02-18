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

QT_BEGIN_NAMESPACE

namespace U1dbDatabase {

QString
Database::getReplicaUid()
{
    QSqlQuery query (m_db.exec("SELECT value FROM u1db_config WHERE name = 'replica_uid'"));
    if (!query.lastError().isValid() && query.next())
        return query.value(0).toString();
    qDebug() << "u1db: Failed to get replica uid:" << query.lastError() << "\n" << query.lastQuery();
    return QString();
}

bool
Database::isInitialized()
{
    m_db.exec("PRAGMA case_sensitive_like=ON");
    QSqlQuery query(m_db.exec(
        "SELECT value FROM u1db_config WHERE name = 'sql_schema'"));
    return query.next();
}

void
Database::initializeIfNeeded(const QString& path)
{
    if (m_db.isOpen())
        return;

    if (!m_db.isValid())
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    // QSqlDatabase will print diagnostics, just bail out
    if (!m_db.isValid())
        return;
    m_db.setDatabaseName(path);
    if (!m_db.open())
    {
        qDebug() << "u1db: Failed to open" << path << ":" << m_db.lastError();
        return;
    }
    if (!isInitialized())
    {
        m_db.exec("BEGIN EXCLUSIVE");
        if (!isInitialized())
        {
            QFile file("../dbschema.sql"); // FIXME: qrc
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                while (!file.atEnd())
                {
                    QByteArray line = file.readLine();
                    while (!line.endsWith(";\n") && !file.atEnd())
                        line += file.readLine();
                    if (m_db.exec(line).lastError().isValid())
                        qDebug() << "u1db: Failed to apply internal schema:" << m_db.lastError() << "\n" << line;
                }

                QSqlQuery query(m_db.exec());
                query.prepare("INSERT OR REPLACE INTO u1db_config VALUES ('replica_uid', :uuid)");
                query.bindValue(":uuid", QUuid::createUuid().toString());
                if (!query.exec())
                    qDebug() << "u1db: Failed to apply internal schema:" << query.lastError() << "\n" << query.lastQuery();
                // Double-check
                if (query.boundValue(0).toString() != getReplicaUid())
                    qDebug() << "u1db: Invalid replica uid:" << query.boundValue(0);

            }
            else
                qDebug() << "u1db: Failed to read internal schema: FileError" << file.error();
        }
    }
}

Database::Database(QObject *parent) :
    QObject(parent), m_path("")
{
    initializeIfNeeded();
}

QVariant
Database::getDoc(const QString& docId, bool checkConflicts)
{
    initializeIfNeeded();

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
        qDebug() << "u1db: Failed to get document" << docId << ": No document";
        return QVariant();
    }
    qDebug() << "u1db: Failed to get document" << docId << ":" << query.lastError() << "\n" << query.lastQuery();
    return QVariant();
}

static int
increaseVectorClockRev(int oldRev)
{
    return oldRev;
}

int
Database::putDoc(const QString& docId, QVariant newDoc)
{
    initializeIfNeeded();

    // FIXME verify Id ^[a-zA-Z0-9.%_-]+$
    QVariant oldDoc = QVariant();//getDoc(docId, true);
    if (oldDoc.isValid() /*&& oldDoc.has_conflicts*/)
        return -1; /* Error: conflicts */

    int newRev = increaseVectorClockRev(7/*newDoc.rev*/);
    QSqlQuery query(m_db.exec());
    if (oldDoc.isValid())
    {
        query.prepare("UPDATE document SET doc_rev=:docRev, content=:docJson WHERE doc_id = :docId");
        query.bindValue(":docId", docId);
        query.bindValue(":docRev", newRev);
        query.bindValue(":docJson", QJsonDocument::fromVariant(newDoc).toJson());
        if (!query.exec())
            qDebug() << "u1db: Failed to put/ update document" << docId << ":" << query.lastError() << "\n" << query.lastQuery();
        query.prepare("DELETE FROM document_fields WHERE doc_id = :docId");
        query.bindValue(":docId", docId);
        if (!query.exec())
            qDebug() << "u1db: Failed to delete document field" << docId << ":" << query.lastError() << "\n" << query.lastQuery();
    }
    else
    {
        query.prepare("INSERT INTO document (doc_id, doc_rev, content) VALUES (:docId, :docRev, :docJson)");
        query.bindValue(":docId", docId);
        query.bindValue(":docRev", newRev);
        query.bindValue(":docJson", QJsonDocument::fromVariant(newDoc).toJson());
        if (!query.exec())
            qDebug() << "u1db: Failed to put document" << docId << ":" << query.lastError() << "\n" << query.lastQuery();
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
    qDebug() << "u1db: Failed to list documents:" << query.lastError() << "\n" << query.lastQuery();
    return QList<QVariant>();
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

} // namespace U1dbDatabase

QT_END_NAMESPACE

#include "moc_database.cpp"

