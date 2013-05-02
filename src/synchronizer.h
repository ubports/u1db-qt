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
#include <QSqlDatabase>
#include <QVariant>

#include "database.h"

QT_BEGIN_NAMESPACE_U1DB

class Q_DECL_EXPORT Synchronizer : public QObject {
    Q_OBJECT
#ifdef Q_QDOC
    Q_PROPERTY(Database source READ getSource WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Database target READ getTarget WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(Database resolver READ getResolver WRITE setResolver NOTIFY resolverChanged)
#else
    Q_PROPERTY(QT_PREPEND_NAMESPACE_U1DB(Database*) source READ getSource WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QT_PREPEND_NAMESPACE_U1DB(Database*) target READ getTarget WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QT_PREPEND_NAMESPACE_U1DB(Database*) resolver READ getResolver WRITE setResolver NOTIFY resolverChanged)
#endif
    Q_PROPERTY(bool synchronize READ getSync WRITE setSync NOTIFY syncChanged)

public:
    Synchronizer(QObject* parent = 0);
    Database* getSource();
    Database* getTarget();
    Database* getResolver();
    bool getSync();
    void setSource(Database* source);
    void setTarget(Database* target);
    void setResolver(Database* resolver);
    void setSync(bool synchronize);
    
Q_SIGNALS:
    void sourceChanged(Database* source);
    void targetChanged(Database* target);
    void resolverChanged(Database* resolver);
    void syncChanged(bool synchronize);
private:
    Q_DISABLE_COPY(Synchronizer)
    Database* m_source;
    Database* m_target;
    Database* m_resolver;
    bool m_synchronize;

};

QT_END_NAMESPACE_U1DB

#endif // U1DB_SYNCHRONIZER_H

