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

#include <QDebug>
#include <QSqlQuery>
#include <QFile>
#include <QSqlError>
#include <QUuid>
#include <QStringList>
#include <QJsonDocument>

#include "synchronizer.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

/*!
    \class Synchronizer
    \inmodule U1db
    \ingroup modules

    \brief The Synchronizer class handles synchronizing between two databases.

*/

/*

 Below this line are methods that are standards in declarative API design. They include the element instantiation, default settings for properties and get/set methods for properties.

*/

/*!
    Instantiate a new Synchronizer with an optional \a parent, usually by declaring it as a QML item.

    Synchronizer elements sync two databases together, a 'source' database and a remote or local 'target' database.

 */

Synchronizer::Synchronizer(QObject *parent) :
    QObject(parent), m_synchronize(false)
{
    qDebug()<<"Synchronizer::Synchronizer";

    QObject::connect(this, &Synchronizer::syncChanged, this, &Synchronizer::onSyncChanged);
}

/*!
    \property Synchronizer::source
 */
void Synchronizer::setSource(Database* source)
{
    qDebug()<<"Synchronizer::setSource";

    if (m_source == source)
        return;

    if (m_source)
        QObject::disconnect(m_source, 0, this, 0);

    m_source = source;

    Q_EMIT sourceChanged(source);
}

/*!
    \property Synchronizer::local_targets
 */
void Synchronizer::setLocalTargets(QList<QObject*> local_targets)
{
    qDebug()<<"Synchronizer::setLocalTargets";

    if (m_local_targets == local_targets)
        return;

    //if (m_local_targets)
      //  QObject::disconnect(m_local_targets, 0, this, 0);

    m_local_targets = local_targets;
    Q_EMIT localTargetsChanged(local_targets);
}

/*!
    \property Synchronizer::remote_targets
 */
void Synchronizer::setRemoteTargets(QList<QString> remote_targets)
{
    qDebug()<<"Synchronizer::setRemoteTargets";

    if (m_remote_targets == remote_targets)
        return;

    //if (m_local_targets)
      //  QObject::disconnect(m_local_targets, 0, this, 0);

    m_remote_targets = remote_targets;
    Q_EMIT remoteTargetsChanged(remote_targets);
}

/*!
    \property Synchronizer::synchronize
 */
void Synchronizer::setSync(bool synchronize)
{
    qDebug()<<"Synchronizer::setSync";

    if (m_synchronize == synchronize)
        return;

    m_synchronize = synchronize;
    Q_EMIT syncChanged(synchronize);
}

/*!
    \property Synchronizer::resolve_to_source
 */
void Synchronizer::setResolveToSource(bool resolve_to_source)
{
    qDebug()<<"Synchronizer::setResolveToSource";

    if (m_resolve_to_source == resolve_to_source)
        return;

    m_resolve_to_source = resolve_to_source;
    Q_EMIT resolveToSourceChanged(resolve_to_source);
}

/*!

 */
Database* Synchronizer::getSource()
{
    qDebug()<<"Synchronizer::getSource";

    return m_source;
}

/*!

 */
QList<QObject*> Synchronizer::getLocalTargets()
{
    qDebug()<<"Synchronizer::getLocalTargets";

    return m_local_targets;
}

/*!

 */
QList<QString> Synchronizer::getRemoteTargets()
{
    qDebug()<<"Synchronizer::getRemoteTargets";

    return m_remote_targets;
}

/*!

 */
bool Synchronizer::getSync()
{
    qDebug()<<"Synchronizer::getSync";

    return m_synchronize;
}

/*!

 */
bool Synchronizer::getResolveToSource(){

    qDebug()<<"Synchronizer::getResolveToSource";

    return m_resolve_to_source;
}

/*

 Below this line represents the unique API methods that are not part of standard declarative API design.

*/

void Synchronizer::onSyncChanged(bool synchronize){

    qDebug() << "Synchronizer::onSyncChanged = " << synchronize;

    if(synchronize == true){

        Database* source = getSource();

/* The source replica asks the target replica for the information it has stored about the last time these two replicas were synchronised (if ever).*/

        QList<QObject*> local_targets = this->getLocalTargets();

        Q_FOREACH (QObject* local_target, local_targets)
        {

            Database *target = (Database*)local_target;
            qDebug() << "Synchronizer::onSyncChanged Q_FOREACH (Database* local_target, local_targets) = " << target->getPath();

            //syncWithLocalTarget(Database *source, Database *target, bool resolve_to_source)

        }

        QList<QString> remote_targets = this->getRemoteTargets();

        Q_FOREACH (QString remote_target, remote_targets)
        {

            qDebug() << "Synchronizer::onSyncChanged Q_FOREACH (QObject* remote_target, remote_targets) = " << remote_target;

            //syncWithRemoteTarget(Database *source, QString target_url, bool resolve_to_source)

        }

        QMap<QString, QVariant> sync_from_information;
        //sync_from_information.insert("source_replica_uid",source_replica_uid);
        sync_from_information.insert("source_replica_generation","");
        sync_from_information.insert("source_transaction_id","");
        //sync_from_information.insert("target_replica_uid",target_replica_uid);
        sync_from_information.insert("target_replica_generation","");
        sync_from_information.insert("target_replica_transaction_id","");

/*

The application wishing to synchronise sends the following GET request to the server:

GET /thedb/sync-from/my_replica_uid

Where thedb is the name of the database to be synchronised, and my_replica_uid is the replica id of the application’s (i.e. the local, or synchronisation source) database

*/

/*

The target responds with a JSON document that looks like this:

{
    "target_replica_uid": "other_replica_uid",
    "target_replica_generation": 12,
    "target_replica_transaction_id": "T-sdkfj92292j",
    "source_replica_uid": "my_replica_uid",
    "source_replica_generation": 23,
    "source_transaction_id": "T-39299sdsfla8"
}

With all the information it has stored for the most recent synchronisation between itself and this particular source replica. In this case it tells us that the synchronisation target believes that when it and the source were last synchronised, the target was at generation 12 and the source at generation 23.

*/



/* The source replica validates that its information regarding the last synchronisation is consistent with the target’s information, and raises an error if not. (This could happen for instance if one of the replicas was lost and restored from backup, or if a user inadvertently tries to synchronise a copied database.) */



/* The source replica generates a list of changes since the last change the target replica knows of. */



/* The source replica checks what the last change is it knows about on the target replica. */



/* If there have been no changes on either replica that the other side has not seen, the synchronisation stops here. */



/* The source replica sends the changed documents to the target, along with what the latest change is that it knows about on the target replica. */



/* The target processes the changed documents, and records the source replica’s latest change. */



/* The target responds with the documents that have changes that the source does not yet know about. */



/* The source processes the changed documents, and records the target replica’s latest change. */



/* If the source has seen no changes unrelated to the synchronisation during this whole process, it now sends the target what its latest change is, so that the next synchronisation does not have to consider changes that were the result of this one.*/

        setSync(false);


    }
    else{

    }
}


QT_END_NAMESPACE_U1DB

#include "moc_synchronizer.cpp"

