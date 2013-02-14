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

//global.h begin
#ifndef QT_STATIC
# if defined(QT_BUILD_U1DB_LIB)
#  define Q_U1DB_EXPORT Q_DECL_EXPORT
# else
#  define Q_U1DB_EXPORT Q_DECL_IMPORT
# endif
#else
# define Q_U1DB_EXPORT
#endif

#if defined(QT_NAMESPACE)
#  define QT_BEGIN_NAMESPACE_U1DB namespace QT_NAMESPACE { namespace U1dbDatabase {
#  define QT_END_NAMESPACE_U1DB } }
#  define QT_USE_NAMESPACE_U1DB using namespace QT_NAMESPACE::U1dbDatabase;
#  define QT_PREPEND_NAMESPACE_U1DB(name) QT_NAMESPACE::U1DbDatabase::name
#else
#  define QT_BEGIN_NAMESPACE_U1DB namespace U1DbDatabase {
#  define QT_END_NAMESPACE_U1DB }
#  define QT_USE_NAMESPACE_U1DB using namespace U1dbDatabase;
#  define QT_PREPEND_NAMESPACE_U1DB(name) U1dbDatabase::name
#endif

// a workaround for moc - if there is a header file that doesn't use u1db
// namespace, we still force moc to do "using namespace" but the namespace have to
// be defined, so let's define an empty namespace here
QT_BEGIN_NAMESPACE_U1DB
QT_END_NAMESPACE_U1DB
//global.h end

#include <QtCore/QObject>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE

namespace U1dbDatabase {

class Q_DECL_EXPORT Database : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
public:
    Database(QObject* parent = 0);
    ~Database() { }

    QString getPath();
    void setPath(const QString& path);
    Q_INVOKABLE QVariant getDoc(const QString& docId, bool checkConflicts);
    Q_INVOKABLE int putDoc(const QString& docID, QVariant newDoc);
    Q_INVOKABLE QList<QVariant> listDocs();
Q_SIGNALS:
    void pathChanged(const QString& path);
private:
    Q_DISABLE_COPY(Database)
    QString m_path;
    QSqlDatabase m_db;

    QString getReplicaUid();
    bool isInitialized();
    void initializeIfNeeded();
};

} // namespace U1dbDatabase

QT_END_NAMESPACE

#endif // U1DB_DATABASE_H

