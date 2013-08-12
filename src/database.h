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

#ifndef U1DB_DATABASE_H
#define U1DB_DATABASE_H

#include "global.h"

#include <QtCore/QObject>
#include <QSqlDatabase>
#include <QVariant>
#include <QAbstractListModel>

QT_BEGIN_NAMESPACE_U1DB

class Q_DECL_EXPORT Database : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString error READ lastError NOTIFY errorChanged)
public:
    Database(QObject* parent = 0);


    // QAbstractListModel
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray>roleNames() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    void resetModel();

    QString getPath();
    void setPath(const QString& path);
    Q_INVOKABLE QVariant getDoc(const QString& docId);
    QString getDocumentContents(const QString& docId);
    QVariant getDocUnchecked(const QString& docId) const;
    Q_INVOKABLE QString putDoc(QVariant newDoc, const QString& docID=QString());
    Q_INVOKABLE QList<QString> listDocs();
    Q_INVOKABLE QString lastError();
    Q_INVOKABLE QString putIndex(const QString& index_name, QStringList expressions);
    Q_INVOKABLE QStringList getIndexExpressions(const QString& indexName);
    Q_INVOKABLE QStringList getIndexKeys(const QString& indexName);

    /* Functions handy for Synchronization */
    QString getNextDocRevisionNumber(QString doc_id);
    QString getCurrentDocRevisionNumber(QString doc_id);
    void updateDocRevisionNumber(QString doc_id,QString revision);
    void updateSyncLog(bool insert, QString uid, QString generation, QString transaction_id);
    QList<QString> listTransactionsSince(int generation);
    QMap<QString,QVariant> getSyncLogInfo(QMap<QString,QVariant> lastSyncInformation, QString uid, QString prefix);

Q_SIGNALS:
    void pathChanged(const QString& path);
    void errorChanged(const QString& error);
    /*!
        A document's contents were modified.
     */
    void docChanged(const QString& docId, QVariant content);
    /*!
        A document was loaded via its docID.
     */
    void docLoaded(const QString& docId, QVariant content) const;
private:
    //Q_DISABLE_COPY(Database)
    QString m_path;
    QSqlDatabase m_db;
    QString m_error;

    QString getReplicaUid();
    bool isInitialized();
    bool initializeIfNeeded(const QString& path=":memory:");
    bool setError(const QString& error);
    QString getDocIdByRow(int row) const;

    int createNewTransaction(QString doc_id);
    QString generateNewTransactionId();
    int getCurrentGenerationNumber();

};

QT_END_NAMESPACE_U1DB

#endif // U1DB_DATABASE_H

