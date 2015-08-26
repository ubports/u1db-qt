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

#include <QStringList>

#include "index.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

/*!
    \class Index
    \inmodule U1db
    \ingroup cpp

    \brief The Index class defines an index to be stored in the database and
    queried using Query. Changes in documents affected by the index also update
    the index in the database.
*/

/*!
    \qmltype Index
    \instantiates Index
    \inqmlmodule U1db 1.0
    \ingroup modules

    \brief An Index defines what fields can be filtered using Query.

    Documents in the database will be included if they contain all fields in the expression.

    \qml
    Index {
        database: myDatabase
        name: 'colorIndex'
        expression: [ 'color' ]
    }
    \endqml

    \sa Query
*/

/*!
    Instantiate a new Index with an optional \a parent,
    usually by declaring it as a QML item.
 */
Index::Index(QObject *parent) :
    QObject(parent), m_database(0)
{
}

/*!
    Returns the \l Database to lookup documents from and store the index in.
 */
Database*
Index::getDatabase()
{
    return m_database;
}

void
Index::onPathChanged(const QString& path)
{
    m_database->putIndex(m_name, m_expression);
    Q_EMIT dataInvalidated();
}

void
Index::onDocChanged(const QString& docId, QVariant content)
{
    Q_EMIT dataInvalidated();
}

/*!
    \qmlproperty Database Index::database
    Sets the Database to lookup documents from and store the index in. The
    dataInvalidated() signal will be emitted on all changes that could affect
    the index.
 */
/*!
    Sets the \a database to lookup documents from and store the index in. The
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

/*!
    Returns the name of the index. Both name and expression must be specified.
 */
QString
Index::getName()
{
    return m_name;
}

/*!
    \qmlproperty string Index::name
    Sets the name used. Both an expression and a name must be specified
    for an index to be created.
 */
/*!
    Sets the \a name used. Both an expression and a name must be specified
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

/*!
    Returns the expression of the index. Both name and expression must be specified.
 */
QStringList
Index::getExpression()
{
    return m_expression;
}

/*!
    \qmlproperty list<string> Index::expression
    Sets the expression used. Both an expression and a name must be specified
    for an index to be created.

    Also starts the process of creating the Index result list, which can then be queried or populate the Query model as is.
 */
/*!
    Sets the \a expression used. Both an expression and a name must be specified
    for an index to be created.

    Also starts the process of creating the Index result list, which can then be queried or populate the Query model as is.
 */
void
Index::setExpression(QStringList expression)
{

    if (m_expression == expression)
        return;

    m_expression = expression;

    if (m_database)
    {
        m_database->putIndex(m_name, m_expression);
        Q_EMIT dataInvalidated();
    }
   
    Q_EMIT expressionChanged(expression);
}

/*!
   \internal
 * Iterates through the documents stored in the database and creates the list of results based on the Index expressions.
 */

void Index::generateIndexResults()
{
    m_results.clear();

    Database *db(getDatabase());

    if(db){

        QList<QString> documents = db->listDocs();

        Q_FOREACH (QString docId, documents){

            QVariant document = db->getDocUnchecked(docId);

            QStringList fieldsList;

            appendResultsFromMap(docId, fieldsList, document.toMap(),"");

        }

    }

}

/*!
   \internal
 */
QList<QVariantMap> Index::getAllResults(){
    generateIndexResults();
    return m_results;
}

/*!
   \internal
 *
 *This method is desinged to recursively iterate through a document, or section of a document, which represents a QVariantMap. As it iterates through the entire document, the method keeps track of the current index expression, and populates a local QVariantMap should the current expression be found in the Index's list of expressions.
 *
 *If that QVariantMap contains more than one entry it is added to the global results, which can then be utilized by a Query. This needs to be modified to ensure all expressions are found, whereas at the moment if more than one expressions are defined and any of them are found then the map is added to the results list.
 *
 */
QStringList Index::appendResultsFromMap(QString docId, QStringList fieldsList, QVariantMap current_section, QString current_field)
{
    QMapIterator<QString, QVariant> i(current_section);

    QString original_field = current_field;

    QVariantMap results_map;

    while (i.hasNext()) {

        i.next();

        if(!original_field.isEmpty()){
            current_field = original_field + "." + i.key();
        }
        else{
            current_field = i.key();
        }

        fieldsList.append(current_field);

        QVariant value = i.value();

        if(value.userType()==8) // QVariantMap
        {
            fieldsList = appendResultsFromMap(docId, fieldsList, value.toMap(),current_field);
        }
        else if(value.userType()==9) // QVariantList
        {
            fieldsList = getFieldsFromList(docId, fieldsList, value.toList(),current_field);
        }

        {
            if(m_expression.contains(current_field)==true){
                results_map.insert(i.key(),value);
            }

        }
    }

    if(results_map.count()>0){
        QVariantMap mapIdResult;
        mapIdResult.insert("docId", docId);
        mapIdResult.insert("result", results_map);
        m_results.append(mapIdResult);
    }

    return fieldsList;
}
/*!
   \internal
 *
 *This recursive method is used in conjuntion with Index::appendResultsFromMap, to aid in iterating through a document when an embedded list is found.
 *
 */
QStringList Index::getFieldsFromList(QString docId, QStringList fieldsList, QVariantList current_section, QString current_field)
{

    QListIterator<QVariant> i(current_section);

    while (i.hasNext()) {

        QVariant value = i.next();

        if(value.userType()==8) // QVariantMap
        {
            fieldsList = appendResultsFromMap(docId, fieldsList, value.toMap(),current_field);
        }
        else if(value.userType()==9) // QVariantList
        {
            fieldsList = getFieldsFromList(docId, fieldsList, value.toList(),current_field);
        }
        else if(value.userType()==10) // QString
        {
            fieldsList.append(current_field);
        }
        else
        {

        }
    }

    return fieldsList;
}

QT_END_NAMESPACE_U1DB

#include "moc_index.cpp"

