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

#include <QtCore/QObject>
#include <QSqlDatabase>
#include <QVariant>

#include "global.h"

QT_BEGIN_NAMESPACE

namespace U1dbDatabase {

class Q_DECL_EXPORT Database : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString error READ lastError NOTIFY errorChanged)
public:
    Database(QObject* parent = 0);
    ~Database() { }

    QString getPath();
    void setPath(const QString& path);
    Q_INVOKABLE QVariant getDoc(const QString& docId, bool checkConflicts, bool fallbackToEmpty=false);
    Q_INVOKABLE int putDoc(QVariant newDoc, const QString& docID=QString());
    Q_INVOKABLE QList<QVariant> listDocs();
    Q_INVOKABLE QString lastError();
Q_SIGNALS:
    void pathChanged(const QString& path);
    void errorChanged(const QString& error);
private:
    Q_DISABLE_COPY(Database)
    QString m_path;
    QSqlDatabase m_db;
    QString m_error;

    QString getReplicaUid();
    bool isInitialized();
    bool initializeIfNeeded(const QString& path=":memory:");
    bool setError(const QString& error);
};

} // namespace U1dbDatabase

QT_END_NAMESPACE

#endif // U1DB_DATABASE_H

