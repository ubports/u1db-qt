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

#include "query.h"
#include "database.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

/*!
    \class Query
    \inmodule U1db
    \ingroup modules

    \brief The Query class generates a filtered list of documents based on a query using the given Index.

    Query can be used as a QAbstractListModel, delegates will then have access to \a docId and \a contents
    analogous to the properties of Document.
*/

/*!
    Instantiate a new Query with an optional \a parent,
    usually by declaring it as a QML item.
 */
Query::Query(QObject *parent) :
    QAbstractListModel(parent), m_index(0)
{
}

/*!
    \internal
 *Used to implement QAbstractListModel
 *Implements the variables exposed to the Delegate in a model
 */
QVariant
Query::data(const QModelIndex & index, int role) const
{

    if (role == 0) // contents
        return m_results.at(index.row());
    if (role == 1) // docId
        return m_documents.at(index.row());
    return QVariant();
}

/*!
    \internal
    Used to implement QAbstractListModel
    Defines \b{contents} and \b{docId} as variables exposed to the Delegate in a model
    \b{index} is supported out of the box.
 */
QHash<int, QByteArray>
Query::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(0, "contents");
    roles.insert(1, "docId");
    return roles;
}

/*!
    \internal
    Used to implement QAbstractListModel
    The number of rows: the number of documents given by the query.
 */
int
Query::rowCount(const QModelIndex & parent) const
{
    return m_results.count();
}

Index*
Query::getIndex()
{
    return m_index;
}

/*!
    Emitted whenever the index or documents change, and the results
    need to be updated.
 */
void
Query::onDataInvalidated()
{
    m_documents.clear();
    m_results.clear();

    if (!m_index)
        return;
    generateQueryResults();

}

/*!
    \internal
    Manually triggers reloading of the query.
 */
void Query::generateQueryResults()
{
    QList<QVariantMap> results(m_index->getAllResults());
    Q_FOREACH (QVariantMap mapIdResult, results) {
        QString docId((mapIdResult["docId"]).toString());
        QVariant result_variant(mapIdResult["result"]);
        QVariantMap result(result_variant.toMap());

        QMapIterator<QString,QVariant> j(result);

        bool match = true;

        while(j.hasNext()){

            j.next();

            if (!queryField(j.key(), j.value())) {
                match = false;
                break;
            }

        }

        if(match == true){
            // Results must be unique and not empty aka deleted
            if (!m_documents.contains(docId) && result_variant.isValid())
            {
                m_documents.append(docId);
                m_results.append(m_index->getDatabase()->getDocUnchecked(docId));
            }
        }

    }

    resetModel();

    Q_EMIT documentsChanged(m_documents);
    Q_EMIT resultsChanged(m_results);

}

/*!
 * \brief Query::resetModel
 *
 * Resets the model of the Query
 *
 */

void Query::resetModel(){
    beginResetModel();
    endResetModel();
}

/*!
    \internal
    Query a single field.
 */
bool Query::queryField(QString field, QVariant value){
    QVariant query = getQuery();
    // * is the default if query is empty
    if (!query.isValid())
        query = QVariant(QString("*"));
    QString typeName = query.typeName();

    if(typeName == "QString")
        return queryString(query.toString(), value);
    else if(typeName == "int")
        return queryString(query.toString(), value);
    else if(typeName == "QVariantList")
        return iterateQueryList(query, field, value);
    else
    {
        m_query = "";
        qWarning("u1db: Unexpected type %s for query", qPrintable(typeName));
    }

    return true;

}

/*!
    \internal
    Loop through the query assuming it's a list.
 */
bool Query::iterateQueryList(QVariant query, QString field, QVariant value)
{
    QList<QVariant> query_list = query.toList();
    QListIterator<QVariant> j(query_list);

    while (j.hasNext()) {

        QVariant j_value = j.next();

        QString typeName = j_value.typeName();

        if(typeName == "QVariantMap")
        {
            if (!queryMap(j_value.toMap(), value.toString(), field))
                return false;
        }
        else if(typeName == "QString"){
            if (!queryString(j_value.toString(), value))
                return false;
        }
        else
        {
            m_query = "";
            qWarning("u1db: Unexpected type %s for query", qPrintable(typeName));
        }

    }

    return true;
}

/*!
    \internal
    Verify that query is an identical or wild card match.
 */
bool Query::queryMatchesValue(QString query, QString value)
{
    if (query == "*")
        return true;
    if (query == value)
        return true;
    if (!query.contains ("*"))
       return false;
    QString prefix(query.split("*")[0]);
    return value.startsWith(prefix, Qt::CaseSensitive);
}

/*!
    \internal
    Handle different types of string values including wildcards.
 */
bool Query::queryString(QString query, QVariant value)
{

    QString typeName = value.typeName();
    if (typeName == "QVariantList") {
        Q_FOREACH (QVariant value_string, value.toList()) {
            if (queryString(query, value_string.toString()))
                return true;
        }
        return false;
    }

    return queryMatchesValue(query, value.toString());
}

/*!
    \internal
    Loop through the given map of keys and queries.
 */
bool Query::queryMap(QVariantMap map, QString value, QString field)
{
    QMapIterator<QString,QVariant> k(map);

    while(k.hasNext()){
        k.next();

        QString k_key = k.key();
        QVariant k_variant = k.value();
        QString query = k_variant.toString();

        if(field == k_key){

            if (!queryMatchesValue(query, value))
                return false;
        }
    }

    return true;
}

/*!
    \property Query::index
    Sets the Index to use. The index must have a valid name and index expressions.
    If no query is set, the default is all results of the index.
 */
void
Query::setIndex(Index* index)
{
    if (m_index == index)
        return;

    if (m_index)
        QObject::disconnect(m_index, 0, this, 0);
    m_index = index;
    if (m_index){
        QObject::connect(m_index, &Index::dataInvalidated, this, &Query::onDataInvalidated);
    }
    Q_EMIT indexChanged(index);


    onDataInvalidated();

}

QVariant
Query::getQuery()
{
    return m_query;
}

/*!
    \property Query::query
    A query in one of the allowed forms:
    'value', ['value'] or [{'sub-field': 'value'}].
    The default is equivalent to '*'.
 */
void
Query::setQuery(QVariant query)
{
    if (m_query == query)
        return;

    m_query = query;
    Q_EMIT queryChanged(query);
    onDataInvalidated();
}

/*!
    \property Query::documents
    The docId's of all matched documents.
 */
QStringList
Query::getDocuments()
{
    return m_documents;
}

/*!
    \property Query::results
    The results of the query as a list.
 */
QList<QVariant>
Query::getResults()
{
    return m_results;
}

QT_END_NAMESPACE_U1DB

#include "moc_query.cpp"

