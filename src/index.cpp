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

#include "index.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

/*!
    \class Index
    \inmodule U1Db
    \ingroup modules

    \brief The Index class defines an index to be stored in the database and
    queried using Query. Changes in documents affected by the index also update
    the index in the database.

    This is the declarative API equivalent of Database::putIndex() and
    Database::getIndexExpressions().
*/

Index::Index(QObject *parent) :
    QObject(parent), m_database(0)
{
}

Database*
Index::getDatabase()
{
    return m_database;
}

void
Index::onPathChanged(const QString& path)
{
    Q_EMIT dataInvalidated();
}

void
Index::onDocChanged(const QString& docId, QVariant content)
{
    Q_EMIT dataInvalidated();
}

/*!
    Sets the Database to lookup documents from and store the index in. The
    dataInvalidated() signal will be emitted on all changes that could affect
    the index.
 */
void
Index::setDatabase(Database* database)
{
    if (m_database == database)
        return;

    if (m_database)
        QObject::disconnect(m_database, 0, this, 0);

    m_database = database;
    Q_EMIT databaseChanged(database);

    if (m_database)
    {
        m_database->putIndex(m_name, m_expression);
        QObject::connect(m_database, &Database::pathChanged, this, &Index::onPathChanged);
        QObject::connect(m_database, &Database::docChanged, this, &Index::onDocChanged);
        Q_EMIT dataInvalidated();
    }

}

QString
Index::getName()
{
    return m_name;
}

/*!
    Sets the name used. Both an expression and a name must be specified
    for an index to be created.
 */
void
Index::setName(const QString& name)
{
    if (m_name == name)
        return;

    if (m_database)
    {
        m_database->putIndex(name, m_expression);
        Q_EMIT dataInvalidated();
    }

    m_name = name;
    Q_EMIT nameChanged(name);
}

QStringList
Index::getExpression()
{
    return m_expression;
}

/*!
    Sets the expression used. Both an expression and a name must be specified
    for an index to be created.

    Also starts the process of creating the Index result list, which can then be queried or populate the Query model as is.

 */
void
Index::setExpression(QStringList expression)
{

    if (m_expression == expression)
        return;

    if (m_database)
    {
        m_database->putIndex(m_name, expression);
        Q_EMIT dataInvalidated();
    }

    m_expression = expression;

    Q_EMIT expressionChanged(expression);
}

/*!
   \internal
 * Iterates through the documents stored in the database and creates the list of results based on the Index expressions.
 */

void Index::generateIndexResults()
{

    Database *db(getDatabase());

    if(db){

        QList<QString> documents = db->listDocs();

        Q_FOREACH (QString docId, documents){

            QVariant document = db->getDocUnchecked(docId);

            QStringList fieldsList;

            appendResultsFromMap(fieldsList, document.toMap(),"");

        }

    }

}

/*!
    \internal
 */
void Index::clearResults()
{
    m_results.clear();
}


/*!
   \internal
 */

QList<QVariantMap> Index::getAllResults(){
    return m_results;
}

/*!
   \internal
 */
QVariantMap Index::getResult(int index){
    return m_results[index];
}

/*!
   \internal
 *
 *This method is desinged to recursively iterate through a document, or section of a document, which represents a QVariantMap. As it iterates through the entire document, the method keeps track of the current index expression, and populates a local QVariantMap should the current expression be found in the Index's list of expressions.
 *
 *If that QVariantMap contains more than one entry it is added to the global results, which can then be utilized by a Query. This needs to be modified to ensure all expressions are found, whereas at the moment if more than one expressions are defined and any of them are found then the map is added to the results list.
 *
 */
QStringList Index::appendResultsFromMap(QStringList fieldsList, QVariantMap current_section, QString current_field)
{

    QMapIterator<QString, QVariant> i(current_section);

    QString original_field = current_field;

    QVariantMap results_map;

    while (i.hasNext()) {

        i.next();

        if(fieldsList.count()>0){
            current_field = original_field + "." + i.key();
        }
        else{
            current_field = i.key();
        }

        fieldsList.append(current_field);

        QVariant value = i.value();

        if(value.userType()==8) // QVariantMap
        {
            fieldsList = appendResultsFromMap(fieldsList, value.toMap(),current_field);
        }
        else if(value.userType()==9) // QVariantList
        {
            fieldsList = getFieldsFromList(fieldsList, value.toList(),current_field);
        }
        else
        {
            if(m_expression.contains(current_field)==true){
                results_map.insert(i.key(),value);
            }

        }
    }

    if(results_map.count()>0){
       m_results.append(results_map);
    }

    return fieldsList;
}
/*!
   \internal
 *
 *This recursive method is used in conjuntion with Index::appendResultsFromMap, to aid in iterating through a document when an embedded list is found.
 *
 */


QStringList Index::getFieldsFromList(QStringList fieldsList, QVariantList current_section, QString current_field)
{

    QListIterator<QVariant> i(current_section);

    while (i.hasNext()) {

        QVariant value = i.next();

        if(value.userType()==8) // QVariantMap
        {
            fieldsList = appendResultsFromMap(fieldsList, value.toMap(),current_field);
        }
        else if(value.userType()==9) // QVariantList
        {
            fieldsList = getFieldsFromList(fieldsList, value.toList(),current_field);
        }
        else
        {

        }
    }

    return fieldsList;
}

QT_END_NAMESPACE_U1DB

#include "moc_index.cpp"

