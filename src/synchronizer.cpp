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
    \inmodule U1Db
    \ingroup modules

    \brief The Synchronizer class handles synchronizing between two databases.

*/

/*

 Below this line are general methods for this class, such as setting/getting values for various properties.

*/

/*!
    Create a new Synchronizer element, with an optional \a parent, usually by declaring it as a QML item.

    Synchronizer elements sync two databases together, a 'source' database and a remote or local 'target' database.

    Example use in a QML application:

    U1db.Synchronizer{
        id: aSynchronizer
        synchronize: false
        source: aDatabase
        targets: [{remote:true,
        ip:"127.0.0.1",
        port: 7777,
        name:"example1.u1db",
        resolve_to_source:true}]

    }

    Short description of properties:

    id: The element's identification.

    bool synchronize: Is the element actively synching or not. Should be set to false.

    U1DB::Database source: The id of a local database that will be used for synchronization.

    QVariant targets: One or more target databases that will be synched with the local database.

    bool targets.remote: Is the target database a remote or local database.

    QString targets.ip: The ip address of a remote database (if applicable).

    int targets.port: Port number of the remote server.

    QString targets.name: The name of the database.

    bool targets.resolve_to_source: In case of conflict should the sync resolve to the source's data (if true).

    Example use with u1db-serve:

    1. In a terminal cd into a directory where the u1db Python reference implemented has been downloaded from lp:u1db.
    2. Using Python create a database called 'example1.u1db' using u1db, and a document 'helloworld':

        # python

            >>> import u1db
            >>> db = u1db.open("example1.u1db",create=True)
            >>> content = {"hello": { "world": { "message":"Hello World Updated" } } }
            >>> db.create_doc(content, doc_id="helloworld")

        ctrl+d

    3. From the u1db directory above type './u1db-serve --port=7777' and hit enter.
    4. Open another terminal tab.
    5. Change into a directory containing u1db-qt (assuming this class is included in that directory and the installed version on the host computer).
    6. Change into the directory where u1db-qt-example-6.qml is located.
    7. Type 'qmlscene u1db-qt-example-6.qml' and hit enter.
    8. Click the button labelled 'Sync'.
    9. Check the terminal windows for output from either the client or server.

 */

Synchronizer::Synchronizer(QObject *parent) :
    QAbstractListModel(parent), m_synchronize(false), m_source(NULL)
{
    QObject::connect(this, &Synchronizer::syncChanged, this, &Synchronizer::onSyncChanged);
}

/*!
    \internal
 *Used to implement QAbstractListModel
 *Implements the variables exposed to the Delegate in a model
 */
QVariant
Synchronizer::data(const QModelIndex & index, int role) const
{
    if (role == 0)
        return m_sync_output.at(index.row());
    return QVariant();
}

/*!
    \internal
    Used to implement QAbstractListModel

    The number of rows: the number of documents given by the query.
 */
int
Synchronizer::rowCount(const QModelIndex & parent) const
{
    return m_sync_output.count();
}

/*!
    \internal
    Used to implement QAbstractListModel

 */
QHash<int, QByteArray>
Synchronizer::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(0, "sync_output");
    return roles;
}


/*!


    Sets the source database.

 */
void Synchronizer::setSource(Database* source)
{

    if (m_source == source)
        return;

    if (m_source)
        QObject::disconnect(m_source, 0, this, 0);

    m_source = source;

    Q_EMIT sourceChanged(source);
}


/*!
 * \property Synchronizer::targets
 *
 * Sets meta-data for databases to be used during a synchronization session.
 *
 * The QVariant is a list that can contain definitions for more than one database
 * to be used as a target. For example:
 *
 * targets: [{remote:true},
 *  {remote:true,
 *  ip:"127.0.0.1",
 *  port: 7777,
 *  name:"example1.u1db",
 *  resolve_to_source:true},
 *  {remote:"OK"}]
 *
 * The above example defines three databases. Two of the three definitions in the
 * example are invalid, the first ({remote:true}) and the third ({remote:"OK"}),
 * because they are incomplete.
 *
 * The second definition is a fully defined and valid definition for a local to
 * remote synchronization of two databases:
 *
 * {remote:true,
 *  ip:"127.0.0.1",
 *  port: 7777,
 *  name:"example1.u1db",
 *  resolve_to_source:true}
 *
 * 'remote' determines whether the database is on disk or located on a server.
 * 'ip' and 'port' for a server are used only when 'remote' is set to true
 * 'name' is the name of the local (on disk) or remote database.
 *  Note: If 'remote' is false this is the relative/absolute file location.
 * 'resolve_to_source' determines whether to resolve conflicts automatically
 * in favor of the source (aka local) database's values or the target's.
 *
 */

void Synchronizer::setTargets(QVariant targets)
{

    if (m_targets == targets)
        return;

    //if (m_targets)
      //  QObject::disconnect(m_targets, 0, this, 0);

    m_targets = targets;
    Q_EMIT targetsChanged(targets);
}

/*!
 * \property Synchronizer::synchronize
 */

void Synchronizer::setSync(bool synchronize)
{

    if (m_synchronize == synchronize)
        return;

    m_synchronize = synchronize;
    Q_EMIT syncChanged(synchronize);
}


/*!
 * \property Synchronizer::resolve_to_source
 */

void Synchronizer::setResolveToSource(bool resolve_to_source)
{
     if (m_resolve_to_source == resolve_to_source)
        return;

    m_resolve_to_source = resolve_to_source;
    Q_EMIT resolveToSourceChanged(resolve_to_source);
}


/*!
 * \fn void Synchronizer::setSyncOutput(QList<QVariant> sync_output)
 *
 * Sets the current value for the active session's \a sync_output.
 *
 */

void Synchronizer::setSyncOutput(QList<QVariant> sync_output)
{
     if (m_sync_output == sync_output)
        return;

    m_sync_output = sync_output;
    Q_EMIT syncOutputChanged(sync_output);
}

/*!
 * \property Synchronizer::source
 *
 *
 *  Returns a source Database.
 *
 */
Database* Synchronizer::getSource()
{
     return m_source;
}

/*!
 * \brief Synchronizer::getTargets
 *
 *
 *  Returns meta-data for all target databases.
 *
 */

QVariant Synchronizer::getTargets()
{
     return m_targets;
}

/*!
 * \brief Synchronizer::getSync
 *
 *
 * Returns the current value of synchronize. If true then the synchronize
 * session is initiated.
 *
 * This should probaby always be set to false on application start up.
 * The application developer should use some trigger to switch it to true
 * when needed (e.g. button click).
 *
 */

bool Synchronizer::getSync()
{
     return m_synchronize;
}

/*!
 * \brief Synchronizer::getResolveToSource
 *
 *
 * If set to true, any document conflicts created during a sync session
 * will be resolved in favor of the content from the source database. If false
 * the content from the target database will replace the document content in
 * the source database.
 *
 */

bool Synchronizer::getResolveToSource(){
    return m_resolve_to_source;
}

/*!
 * \property Synchronizer::sync_output
 * \brief Synchronizer::getSyncOutput
 *
 * Returns the output from a sync session. The list should contain numerous
 * QVariantMaps, each of which will have various meta-data with informative
 * information about what happened in the background of the session.
 *
 * In some cases the information will be about errors or warnings, and in
 * other cases simple log messages. Also included would noramlly be associated
 * properties, elements and other data.
 *
 * The information can be used in any number of ways, such as on screen within an app,
 * testing, console output, logs and more. This is designed to be flexible enough that
 * the app developer can decide themselves how to best use the data.
 *
 */

QList<QVariant> Synchronizer::getSyncOutput(){
    return m_sync_output;
}

/*

 Below this line represents the class' more unique functionality.
 In other words, methods that do more than simply modify/retrieve
 an element's property values.

*/


/*!
 * \brief Synchronizer::onSyncChanged
 *
 * The synchroization process begins here.
 *
 */

void Synchronizer::onSyncChanged(bool synchronize){

    Database* source = getSource();

    QList<QVariant> sync_targets;

    /*!
     * The validator map contains key and value pair definitions,
     *that are used to confirm that the values provided for each
     *database target are of the expected type for a particular key.
     */

    QMap<QString,QString>validator;

    validator.insert("remote","bool");
    validator.insert("location","QString");
    validator.insert("resolve_to_source","bool");


    /*!
     * The mandatory map contains the keys that are used to confirm
     *that a database target definition contains all the mandatory keys
     *necessary for synchronizing.
     */

    QList<QString>mandatory;

    mandatory.append("remote");
    mandatory.append("resolve_to_source");

    if(synchronize == true){

        /*!
          A list of valid sync target databases is generated by calling the getValidTargets(validator, mandatory) method, and adding the return value to a QList (sync_targets).
          */

        sync_targets=getValidTargets(validator, mandatory);

        /*!
         * Once the list of sync targets has been generated the sync activity
         *can be initiated via the synchronizeTargets function.
         */

        synchronizeTargets(source, sync_targets);

        /*!
         * After the synchronization is complete the model is reset so that
         *log and error messages are available at the application level.
         *
         */

        beginResetModel();
        endResetModel();

        /*!
          The convenience signals syncOutputChanged and syncCompleted are
emitted after the model has been reset.
          */

        Q_EMIT syncOutputChanged(m_sync_output);
        Q_EMIT syncCompleted();

        /*!
         * The sync boolean value is reset to its default value (false)
         *once all sync activity is complete.
         */

        setSync(false);

    }
    else{

    }
}


/*!
 * \internal
 * \brief Synchronizer::getValidTargets
 *
 *
 * This method confirms that each sync target definition is valid, based
 * on predefined criteria contained in the validator and mandatory lists.
 *
 */

QList<QVariant> Synchronizer::getValidTargets(QMap<QString,QString>validator, QList<QString>mandatory){

    QList<QVariant> sync_targets;

    int index = 0;

    QList<QVariant> targets = getTargets().toList();

    Q_FOREACH (QVariant target_variant, targets)
    {
        index++;
        QString index_number = QString::number(index);

        QMap<QString, QVariant> target = target_variant.toMap();

        bool valid = true;
        bool complete = true;

        QMapIterator<QString, QVariant> i(target);
        while (i.hasNext()) {

            i.next();

            if(validator.contains(i.key())&&validator[i.key()]!=i.value().typeName()){
                valid = false;

                QString message_value = "For property `" + i.key() + "` Expecting type `" + validator[i.key()] + "`, but received type `" + i.value().typeName()+"`";
                
                QVariantMap output_map;
                output_map.insert("concerning_property","targets");
                output_map.insert("concerning_index",index_number);
                output_map.insert("message_type","error");
                output_map.insert("message_value",message_value);
                m_sync_output.append(output_map);
                
                target.insert("sync",false);

                break;
            }

            if(valid==false){
                targets.removeOne(target);
                break;
            }
            else{
                QListIterator<QString> j(mandatory);

                while(j.hasNext()){
                    QString value = j.next();
                    if(!target.contains(value)){

                        QString message_value = "Expected property `" + value + "`, but it is not present.";

                        QVariantMap output_map;
                        output_map.insert("concerning_property","targets");
                        output_map.insert("concerning_index",index_number);
                        output_map.insert("message_type","error");
                        output_map.insert("message_value",message_value);
                        m_sync_output.append(output_map);

                        target.insert("sync",false);
                        targets.removeOne(target);
                        complete = false;
                        break;
                    }
                }
                if(complete==false){
                    break;
                }

            }
        }

        if(target.contains("sync")&&target["sync"]==false){

            QString message_value = "Not synced due to errors with properties.";

            QVariantMap output_map;
            output_map.insert("concerning_property","targets");
            output_map.insert("concerning_index",index_number);
            output_map.insert("message_type","error");
            output_map.insert("message_value",message_value);
            m_sync_output.append(output_map);

        }
        else
        {
            target.insert("sync",true);
            sync_targets.append(target);

            QString message_value = "Mandatory properties were included and their values are valid.";

            QVariantMap output_map;
            output_map.insert("concerning_property","targets");
            output_map.insert("concerning_index",index_number);
            output_map.insert("message_type","no-errors");
            output_map.insert("message_value",message_value);
            m_sync_output.append(output_map);

        }

    }

    return sync_targets;

}

/*!
 * \internal
 * \brief Synchronizer::synchronizeTargets
 *
 * The source database is synchronized with the target databases contained
 * in the 'targets' list. That list should only contain valid targets, as
 * determined by Synchronizer::getValidTargets.
 *
 */

void Synchronizer::synchronizeTargets(Database *source, QVariant targets){

    if(targets.typeName()== QStringLiteral("QVariantList")){

        QList<QVariant> target_list = targets.toList();

        QListIterator<QVariant> i(target_list);

        int target_index = -1;

        while(i.hasNext()){

            target_index++;

            QVariant target = i.next();

            if(target.typeName()== QStringLiteral("QVariantMap")){
                QMap<QString,QVariant> target_map = target.toMap();

                if(target_map.contains("remote")&&target_map["remote"]==false){
                    if(target_map.contains("sync")&&target_map["sync"]==true){

                        QString message_value = "Valid local target.";

                        QVariantMap output_map;
                        output_map.insert("concerning_property","targets");
                        output_map.insert("concerning_index",target_index);
                        output_map.insert("message_type","no-errors");
                        output_map.insert("message_value",message_value);
                        m_sync_output.append(output_map);

                        syncLocalToLocal(source, target_map);
                    }
                }
                else if(target_map.contains("remote")&&target_map["remote"]==true){
                    if(target_map.contains("sync")&&target_map["sync"]==true){

                        //ip
                        //port
                        //name
                        //GET /thedb/sync-from/my_replica_uid

                        QString source_uid = getUidFromLocalDb(source->getPath());
                        QString get_string = target_map["name"].toString()+"/sync-from/"+source_uid;
                        QString url_string = "http://"+target_map["ip"].toString();
                        QString full_get_request = url_string+"/"+get_string;
                        int port_number = target_map["port"].toInt();

                        QNetworkAccessManager *manager = new QNetworkAccessManager(source);

                        QUrl url(full_get_request);
                        url.setPort(port_number);

                        QNetworkRequest request(url);

                        connect(manager, &QNetworkAccessManager::finished, this, &Synchronizer::remoteGetSyncInfoFinished);

                        QString message_value = "Valid remote target.";

                        QVariantMap output_map;
                        output_map.insert("concerning_property","targets");
                        output_map.insert("concerning_index",target_index);
                        output_map.insert("message_type","no-errors");
                        output_map.insert("message_value",message_value);
                        m_sync_output.append(output_map);

                        manager->get(QNetworkRequest(request));

                    }
                }
                else{

                    QString message_value = "Unknown error. Please check properties";

                    QVariantMap output_map;
                    output_map.insert("concerning_property","targets");
                    output_map.insert("concerning_index",target_index);
                    output_map.insert("message_type","error");
                    output_map.insert("message_value",message_value);
                    m_sync_output.append(output_map);

                }

            }

        }

    }

}

/*!
 * \internal
 * \brief Synchronizer::syncLocalToLocal
 *
 * This function synchronizes two local databases, a source database and a target database.
 *
 */

void Synchronizer::syncLocalToLocal(Database *sourceDb, QMap<QString,QVariant> target)
{

    QString target_db_name = target["location"].toString();

    Database *targetDb;
    Index *targetIndex;
    Query *targetQuery;

    Index *sourceIndex;
    Query *sourceQuery;

    if(target.contains("id")){
        targetDb = (Database*)target["id"].value<QObject*>();
    }

    if(target.contains("target_query")){
        targetQuery = (Query*)target["target_query"].value<QObject*>();
        targetIndex = targetQuery->getIndex();
        targetDb = targetIndex->getDatabase();
    }

    if(target.contains("source_query")){
        sourceQuery = (Query*)target["source_query"].value<QObject*>();
        sourceIndex = sourceQuery->getIndex();
        sourceDb = sourceIndex->getDatabase();
    }

    if(sourceDb == NULL || targetDb == NULL){

        QString message_value = "Either source or target does not exist or is not active.";

        QVariantMap output_map;
        output_map.insert("concerning_property","source|targets");
        //output_map.insert("concerning_index",index_number); // no access to targets index?
        output_map.insert("message_type","error");
        output_map.insert("message_value",message_value);
        m_sync_output.append(output_map);

        return;
    }

    QMap<QString,QVariant> lastSyncInformation;

    lastSyncInformation.insert("target_replica_uid",getUidFromLocalDb(target_db_name));
    lastSyncInformation.insert("target_replica_generation","");
    lastSyncInformation.insert("target_replica_transaction_id",-1);
    lastSyncInformation.insert("source_replica_uid",getUidFromLocalDb(sourceDb->getPath()));
    lastSyncInformation.insert("source_replica_generation","");
    lastSyncInformation.insert("source_replica_transaction_id",-1);

    lastSyncInformation = getLastSyncInformation(sourceDb, targetDb, false, lastSyncInformation);

    QList<QString> transactionsFromSource;
    QList<QString> transactionsFromTarget;

    // Check if target and source have ever been synced before

    if(lastSyncInformation["target_replica_uid"].toString() != "" && lastSyncInformation["target_replica_generation"].toString() != "" && lastSyncInformation["target_replica_transaction_id"].toInt() != -1 && lastSyncInformation["source_replica_uid"].toString() != "" && lastSyncInformation["source_replica_generation"].toString() != "" && lastSyncInformation["source_replica_transaction_id"].toInt() != -1)
    {

        QString message_value = "Source and local database have previously synced.";

        QVariantMap output_map;
        output_map.insert("concerning_property","source|targets");
        output_map.insert("concerning_source",sourceDb->getPath());
        output_map.insert("concerning_target",target_db_name);
        output_map.insert("message_type","no-errors");
        output_map.insert("message_value",message_value);
        m_sync_output.append(output_map);

        //Do some syncing

        transactionsFromSource = sourceDb->listTransactionsSince(lastSyncInformation["source_replica_generation"].toInt());

        transactionsFromTarget = targetDb->listTransactionsSince(lastSyncInformation["target_replica_generation"].toInt());


    }
    else{

        QString message_value = "Source and local database have not previously synced.";

        QVariantMap output_map;
        output_map.insert("concerning_property","source|targets");
        output_map.insert("concerning_source",sourceDb->getPath());
        output_map.insert("concerning_target",target_db_name);
        output_map.insert("message_type","no-errors");
        output_map.insert("message_value",message_value);
        m_sync_output.append(output_map);

        //There is a first time for everything, let's sync!

        transactionsFromSource = sourceDb->listTransactionsSince(0);

        transactionsFromTarget = targetDb->listTransactionsSince(0);
    }

    /*!
     * With two distinct lists present, it is now possible to check what
     * updates should be made, or new documents created in one or the other
     * database, depending on conditions.
     *
     * However, two additional lists containing transactions IDs are required
     * because the information is contained within delimited strings (see
     * below for details).
     *
    */

    QList<QString> transactionIdsFromSource;
    QList<QString> transactionIdsFromTarget;

    Q_FOREACH(QString sourceTransaction, transactionsFromSource){

        /*!
         * Each sourceTransaction is a pipe delimited string containing
         * generation number, document ID, and transaction ID details
         * in that order.
         *
         * Splitting the string into its component pieces provides a
         * document ID (the second key in the list).
         */

        QStringList transactionDetails = sourceTransaction.split("|");

        /*!
          * It is only necessary to have unique instances of the
          * document ID.
          */

        if(!transactionIdsFromSource.contains(transactionDetails[1]))
            transactionIdsFromSource.append(transactionDetails[1]);

    }

    Q_FOREACH(QString targetTransaction, transactionsFromTarget){

        /*!
         * Each targetTransaction is a pipe delimited string containing
         * generation number, document ID, and transaction ID details
         * in that order.
         *
         * Splitting the string into its component pieces provides a
         * document ID (the second key in the list).
         */

        QStringList transactionDetails = targetTransaction.split("|");

        /*!
          * It is only necessary to have unique instances of the
          * document ID.
          */

        if(!transactionIdsFromTarget.contains(transactionDetails[1]))
            transactionIdsFromTarget.append(transactionDetails[1]);

    }   


    /* The source replica asks the target replica for the information it has stored about the last time these two replicas were synchronised (if ever).*/


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


}

/*!
 * \internal
 * \brief Synchronizer::syncDocument
 *
 *
 *  This method is used to synchronize documents from one local database
 * to another local database.
 *
 */


QVariant Synchronizer::syncDocument(Database *from, Database *to, QString docId)
{
    QVariant document = from->getDoc(docId);
    to->putDoc(document, docId);
    QString revision = from->getCurrentDocRevisionNumber(docId);
    to->updateDocRevisionNumber(docId,revision);

    return document;
}

/*!
 * \internal
 * \brief Synchronizer::getLastSyncInformation
 *
 *
 * If the source and target database have ever been synced before the information
 * from that previous session is returned. This is only used for local to local
 * databases. The local to remote procedure is handled elsewhere in a different manner.
 *
 */

QMap<QString,QVariant> Synchronizer::getLastSyncInformation(Database *sourceDb, Database *targetDb, bool remote, QMap<QString,QVariant> lastSyncInformation){

    if(remote == true){

        QString message_value = "Sync information from remote target not available at this time.";

        QVariantMap output_map;
        output_map.insert("concerning_property","source|targets");
        output_map.insert("concerning_source",sourceDb->getPath());
        output_map.insert("message_type","warning");
        output_map.insert("message_value",message_value);
        m_sync_output.append(output_map);

        return lastSyncInformation;

    }
    else{

        lastSyncInformation["source_replica_uid"].toString();

        lastSyncInformation = targetDb->getSyncLogInfo(lastSyncInformation, lastSyncInformation["source_replica_uid"].toString(),"target");

        lastSyncInformation = sourceDb->getSyncLogInfo(lastSyncInformation, lastSyncInformation["target_replica_uid"].toString(),"source");

    }

    return lastSyncInformation;

}

/*!
 * \internal
 * \brief Synchronizer::getUidFromLocalDb
 *
 *
 * The unique id of a database is needed in certain situations of a
 * synchronize session. This method retrieves that id from a local database.
 *
 */

QString Synchronizer::getUidFromLocalDb(QString dbFileName)
{

    QString dbUid;

    QSqlDatabase db;

    db = QSqlDatabase::addDatabase("QSQLITE",QUuid::createUuid().toString());

    QFile db_file(dbFileName);

    if(!db_file.exists())
    {

        QString message_value = "Database does not exist.";

        QVariantMap output_map;
        output_map.insert("concerning_property","source|targets");
        output_map.insert("concerning_database",dbFileName);
        output_map.insert("message_type","error");
        output_map.insert("message_value",message_value);
        m_sync_output.append(output_map);

        return dbUid;
    }
    else
    {
        db.setDatabaseName(dbFileName);
        if (!db.open()){

            QString message_value = db.lastError().text();

            QVariantMap output_map;
            output_map.insert("concerning_property","source|targets");
            output_map.insert("concerning_database",dbFileName);
            output_map.insert("message_type","error");
            output_map.insert("message_value",message_value);
            m_sync_output.append(output_map);

        }
        else{
            QSqlQuery query (db.exec("SELECT value FROM u1db_config WHERE name = 'replica_uid'"));

            if(!query.lastError().isValid() && query.next()){
                dbUid = query.value(0).toString();
                db.close();

                dbUid = dbUid.replace("{","");

                dbUid = dbUid.replace("}","");

                return dbUid;
            }
            else{
                qDebug() << query.lastError().text();
                db.close();
                return dbUid;
            }
        }

    }

    return dbUid;
}

/*!
 * \internal
 * \brief Synchronizer::remoteGetSyncInfoFinished
 *
 * Once the initial exchange between the client application
 * and the remote server is complete, this method retrieves
 * necessary information from the reply that came from the
 * server, and is required for posting data from the client
 * in the steps that will follow.
 *
 * After the data is saved to a string and the network reply
 * is closed, the appropriate method for posting
 * (postDataFromClientToRemoteServer) is then called.
 *
 */

void Synchronizer::remoteGetSyncInfoFinished(QNetworkReply* reply)
{

    QNetworkAccessManager *manager = reply->manager();

    Database *source = qobject_cast<Database *>(manager->parent());

    QUrl postUrl = reply->request().url();

    QByteArray data = reply->readAll();

    QString replyData = QString(data);

    reply->close();

    postDataFromClientToRemoteServer(source, postUrl, replyData);
}


/*!
 * \internal
 * \brief Synchronizer::postDataFromClientToRemoteServer
 *
 * This method builds a string for posting from the client
 * application to the remote server, using information previously
 * gathered from the source and target databases,
 * and then initiates the post.
 *
 */

void Synchronizer::postDataFromClientToRemoteServer(Database *source, QUrl postUrl, QString replyData)
{

    QVariantMap replyMap;

    QJsonDocument replyJson = QJsonDocument::fromJson(replyData.toUtf8());

    QVariant replyVariant = replyJson.toVariant();

    replyMap = replyVariant.toMap();

    double source_replica_generation = replyMap["source_replica_generation"].toDouble();
    QString source_replica_uid = replyMap["source_replica_uid"].toString();
    QString source_replica_transaction_id = replyMap["source_transaction_id"].toString();
    //double target_replica_generation = replyMap["target_replica_generation"].toDouble();
    QString target_replica_transaction_id = replyMap["target_replica_transaction_id"].toString();
    QString target_replica_uid = replyMap["target_replica_uid"].toString();

    QNetworkAccessManager *manager = new QNetworkAccessManager(source);

    connect(manager, &QNetworkAccessManager::finished, this, &Synchronizer::remotePostSyncInfoFinished);

    QByteArray postString;

    postString = "[\r\n";
    postString.append("{\"last_known_generation\": ");
    postString.append(QByteArray::number(source_replica_generation));
    postString.append(", \"last_known_trans_id\": \"");
    postString.append(source_replica_transaction_id);
    postString.append("\"}");

    QList<QString> transactions = m_source->listTransactionsSince(source_replica_generation);

    Q_FOREACH(QString transaction,transactions){
        QStringList transactionData = transaction.split("|");

        QString content = source->getDocumentContents(transactionData[1]);
        content = content.replace("\r\n","");
        content = content.replace("\r","");
        content = content.replace("\n","");
        content = content.replace("\"","\\\"");

        postString.append(",\r\n{\"content\": \""+content+"\",\"rev\": \""+m_source->getCurrentDocRevisionNumber(transactionData[1])+"\", \"id\": \""+transactionData[1]+"\",\"trans_id\": \""+transactionData[2]+"\",\"gen\": "+transactionData[0]+"}");

    }

    postString.append("\r\n]");

    QByteArray postDataSize = QByteArray::number(postString.size());

    QNetworkRequest request(postUrl);
    request.setRawHeader("User-Agent", "U1Db-Qt v1.0");
    request.setRawHeader("X-Custom-User-Agent", "U1Db-Qt v1.0");
    request.setRawHeader("Content-Type", "application/x-u1db-sync-stream");
    request.setRawHeader("Content-Length", postDataSize);

    manager->post(QNetworkRequest(request),postString);

}

/*!
 * \internal
 * \brief Synchronizer::remotePostSyncInfoFinished
 *
 * This method is a slot, which is called once the data
 * from the client application has been posted to the remote
 * server.
 *
 * This is where any new data from the
 * remote server is gathered, and then the further steps
 * for processing on the client side are begun.
 */

void Synchronizer::remotePostSyncInfoFinished(QNetworkReply* reply)
{

    QNetworkAccessManager *manager = reply->manager();

    Database *source = qobject_cast<Database *>(manager->parent());

    QByteArray data = reply->readAll();

    QString replyData = QString(data);

    reply->close();

    processDataFromRemoteServer(source, replyData);

}

/*!
 * \internal
 * \brief Synchronizer::processDataFromRemoteServer
 *
 * After the remote target database has replied back, the data it has sent
 * is processed to determine what action to take on any relevant documents
 * that may have been included in the data.
 *
 */

void Synchronizer::processDataFromRemoteServer(Database *source, QString replyData)
{

    replyData = replyData.replace("\r\n","");

    QJsonDocument replyJson = QJsonDocument::fromJson(replyData.toUtf8());

    QVariant replyVariant = replyJson.toVariant();

    QVariantList replyList = replyVariant.toList();

    QListIterator<QVariant> i(replyList);

    int index = -1;

    while(i.hasNext()){

        index++;

        QVariant current = i.next();

        QString type_name = QString::fromUtf8(current.typeName());

        if(type_name == "QVariantMap")
        {

            QVariantMap map = current.toMap();

            if(index == 0)
            {
                // Meta data
            }
            else
            {
                // Document to update

                QString id("");
                QVariant content("");
                QString rev("");

                QMapIterator<QString, QVariant> i(map);

                while (i.hasNext()) {

                    i.next();

                    if(i.key()=="content")
                    {
                        content = i.value();
                    }
                    else if(i.key()=="id")
                    {
                        id = i.value().toString();
                    }
                    else if(i.key()=="rev")
                    {
                        rev = i.value().toString();
                    }

                }

                if(content!=""&&id!=""&&rev!="")
                {
                    source->putDoc(content,id);
                    source->updateDocRevisionNumber(id,rev);
                }

            }

        }

    }

}

QT_END_NAMESPACE_U1DB

#include "moc_synchronizer.cpp"

