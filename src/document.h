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

#ifndef U1DB_DOCUMENT_H
#define U1DB_DOCUMENT_H

#include <QtCore/QObject>
#include <QVariant>

#include "database.h"

QT_BEGIN_NAMESPACE_U1DB

class Q_DECL_EXPORT Document : public QObject {
    Q_OBJECT
#ifdef Q_QDOC
    /*! database */
    Q_PROPERTY(Database* database READ getDatabase WRITE setDatabase NOTIFY databaseChanged)
#else
    Q_PROPERTY(QT_PREPEND_NAMESPACE_U1DB(Database*) database READ getDatabase WRITE setDatabase NOTIFY databaseChanged)
#endif
    /*! docId */
    Q_PROPERTY(QString docId READ getDocId WRITE setDocId NOTIFY docIdChanged)
    /*! create */
    Q_PROPERTY(bool create READ getCreate WRITE setCreate NOTIFY createChanged)
    /*! defaults */
    Q_PROPERTY(QVariant defaults READ getDefaults WRITE setDefaults NOTIFY defaultsChanged)
    /*! contents */
    Q_PROPERTY(QVariant contents READ getContents WRITE setContents NOTIFY contentsChanged)
public:
    Document(QObject* parent = 0);

    Database* getDatabase();
    void setDatabase(Database* database);
    QString getDocId();
    void setDocId(const QString& docId);
    bool getCreate();
    void setCreate(bool create);
    QVariant getDefaults();
    void setDefaults(QVariant defaults);
    QVariant getContents();
    void setContents(QVariant contents);
Q_SIGNALS:
    /*!
        The database changed.
     */
    void databaseChanged(Database* database);
    /*!
        The docId changed.
     */
    void docIdChanged(const QString& docId);
    /*!
        The create flag changed.
     */
    void createChanged(bool create);
    /*!
        The default contents changed.
     */
    void defaultsChanged(QVariant defaults);
    /*!
        The current contents of the document changed.
     */
    void contentsChanged(QVariant contents);
private:
    Q_DISABLE_COPY(Document)
    Database* m_database;
    QString m_docId;
    bool m_create;
    QVariant m_defaults;
    QVariant m_contents;

    void onDocChanged(const QString& docID, QVariant content);
    void onPathChanged(const QString& path);
};

QT_END_NAMESPACE_U1DB

#endif // U1DB_DOCUMENT_H

