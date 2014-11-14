/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Kevin Wright <kevin.wright@canonical.com>
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

#ifndef U1DB_SYNCHRONIZER_H
#define U1DB_SYNCHRONIZER_H

#include <QtCore/QObject>
#include <QVariant>
#include <QtNetwork>


#include "database.h"
#include "index.h"
#include "query.h"

QT_BEGIN_NAMESPACE_U1DB

class Q_DECL_EXPORT Synchronizer : public QAbstractListModel {
    Q_OBJECT
#ifdef Q_QDOC
    /*! source */
    Q_PROPERTY(Database* source READ getSource WRITE setSource NOTIFY sourceChanged)   
#else
    Q_PROPERTY(QT_PREPEND_NAMESPACE_U1DB(Database*) source READ getSource WRITE setSource NOTIFY sourceChanged)  
#endif
    /*! synchronize */
    Q_PROPERTY(bool synchronize READ getSync WRITE setSync NOTIFY syncChanged)
    /*! resolve_to_source */
    Q_PROPERTY(bool resolve_to_source READ getResolveToSource WRITE setResolveToSource NOTIFY resolveToSourceChanged)
    /*! targets */
    Q_PROPERTY(QVariant targets READ getTargets WRITE setTargets NOTIFY targetsChanged)
    /*! sync_output */
    Q_PROPERTY(QList<QVariant> sync_output READ getSyncOutput NOTIFY syncOutputChanged)

public:
    Synchronizer(QObject* parent = 0);

    // QAbstractListModel
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray>roleNames() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QList<QVariant> getValidTargets(QMap<QString,QString>validator, QList<QString>mandatory);
    QMap<QString,QVariant> getLastSyncInformation(Database *sourceDb, Database *targetDb, bool remote, QMap<QString,QVariant> lastSyncInformation);


    Database* getSource();
    QVariant getTargets();
    bool getSync();
    bool getResolveToSource();
    QList<QVariant> getSyncOutput();

    void setSource(Database* source);
    void setTargets(QVariant targets);
    void setSync(bool synchronize);
    void setResolveToSource(bool resolve_to_source);
    void setSyncOutput(QList<QVariant> sync_output);

    void syncLocalToLocal(Database *sourceDb, QMap<QString,QVariant> target);
    void synchronizeTargets(Database *source, QVariant targets);

    QVariant syncDocument(Database *from, Database *to, QString docId); 
    QString getUidFromLocalDb(QString dbFileName);

    void postDataFromClientToRemoteServer(Database *source, QUrl postUrl, QString replyData);
    void processDataFromRemoteServer(Database *source, QString replyData);


Q_SIGNALS:

    /*!
     * \brief sourceChanged
     * \param source
     */

    void sourceChanged(Database* source);

    /*!
     * \brief targetsChanged
     * \param targets
     */

    void targetsChanged(QVariant targets);

    /*!
     * \brief syncChanged
     * \param synchronize
     */

    void syncChanged(bool synchronize);

    /*!
     * \brief resolveToSourceChanged
     * \param resolve_to_source
     */

    void resolveToSourceChanged(bool resolve_to_source);

    /*!
     * \brief syncOutputChanged
     * \param sync_output
     */

    void syncOutputChanged(QList<QVariant> sync_output);

    /*!
     * \brief syncCompleted
     */

    void syncCompleted();

private:
    //Q_DISABLE_COPY(Synchronizer)
    bool m_synchronize;
    Database* m_source;
    bool m_resolve_to_source;
    QVariant m_targets;
    QList<QVariant> m_sync_output;

    void onSyncChanged(bool synchronize);
    void remoteGetSyncInfoFinished(QNetworkReply* reply);
    void remotePostSyncInfoFinished(QNetworkReply* reply);

};

QT_END_NAMESPACE_U1DB



#endif // U1DB_SYNCHRONIZER_H


