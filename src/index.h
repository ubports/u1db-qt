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

#ifndef U1DB_INDEX_H
#define U1DB_INDEX_H

#include <QtCore/QObject>
#include <QStringList>

#include "database.h"

QT_BEGIN_NAMESPACE_U1DB

class Q_DECL_EXPORT Index : public QObject {
    Q_OBJECT
#ifdef Q_QDOC
    Q_PROPERTY(Database* database READ getDatabase WRITE setDatabase NOTIFY databaseChanged)
#else
    Q_PROPERTY(QT_PREPEND_NAMESPACE_U1DB(Database*) database READ getDatabase WRITE setDatabase NOTIFY databaseChanged)
#endif
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QStringList expression READ getExpression WRITE setExpression NOTIFY expressionChanged)
public:
    Index(QObject* parent = 0);

    Database* getDatabase();
    void setDatabase(Database* database);
    QString getName();
    void setName(const QString& name);
    QStringList getExpression();
    void setExpression(QStringList expression);
    QList<QVariantMap> getAllResults();

Q_SIGNALS:
    void databaseChanged(Database* database);
    void nameChanged(const QString& name);
    void expressionChanged(QVariant expression);
    /*!
        The database, an indexed document or the expressions changed.
     */
    void dataInvalidated();
private:
    Q_DISABLE_COPY(Index)
    Database* m_database;
    QString m_name;
    QStringList m_expression;
    QList<QVariantMap> m_results;

    void onPathChanged(const QString& path);
    void onDocChanged(const QString& docId, QVariant content);

    QStringList appendResultsFromMap(QString docId, QStringList fieldsList, QVariantMap current_section, QString current_field);
    QStringList getFieldsFromList(QString docId, QStringList fieldsList, QVariantList current_section, QString current_field);
    void generateIndexResults();
};

QT_END_NAMESPACE_U1DB

#endif // U1DB_INDEX_H

