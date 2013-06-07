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
    Q_PROPERTY(Database* source READ getSource WRITE setSource NOTIFY sourceChanged)   
#else
    Q_PROPERTY(QT_PREPEND_NAMESPACE_U1DB(Database*) source READ getSource WRITE setSource NOTIFY sourceChanged)  
#endif
    Q_PROPERTY(bool synchronize READ getSync WRITE setSync NOTIFY syncChanged)
    Q_PROPERTY(bool resolve_to_source READ getResolveToSource WRITE setResolveToSource NOTIFY resolveToSourceChanged)
    //Q_PROPERTY(QList <QObject*> local_targets READ getLocalTargets WRITE setLocalTargets NOTIFY localTargetsChanged)
    //Q_PROPERTY(QList <QString> remote_targets READ getRemoteTargets WRITE setRemoteTargets NOTIFY remoteTargetsChanged)
    Q_PROPERTY(QVariant targets READ getTargets WRITE setTargets NOTIFY targetsChanged)

public:
    Synchronizer(QObject* parent = 0);
    Database* getSource();
    QList<QObject*> getLocalTargets();
    QList<QString> getRemoteTargets();
    QVariant getTargets();
    bool getSync();
    bool getResolveToSource();
    void setSource(Database* source);
    void setLocalTargets(QList<QObject*> local_targets);
    void setRemoteTargets(QList<QString> remote_targets);
    void setTargets(QVariant targets);
    void setSync(bool synchronize);
    void setResolveToSource(bool resolve_to_source);

    void syncWithLocalTarget(Database *source, Database *target, bool resolve_to_source);
    void syncWithRemoteTarget(Database *source, QString target_url, bool resolve_to_source);
    void syncLocalToLocal(Database *source, QMap<QString,QVariant> target);
    void synchronizeTargets(Database *source, QVariant targets);

Q_SIGNALS:
    void sourceChanged(Database* source);
    void localTargetsChanged(QList<QObject*> local_targets);
    void remoteTargetsChanged(QList<QString> remote_targets);
    void targetsChanged(QVariant targets);
    void syncChanged(bool synchronize);
    void resolveToSourceChanged(bool resolve_to_source);
private:
    Q_DISABLE_COPY(Synchronizer)
    Database* m_source;
    bool m_synchronize;
    bool m_resolve_to_source;
    QList <QObject*> m_local_targets;
    QList <QString> m_remote_targets;
    QVariant m_targets;
    QList <QStringList> m_errors;

    void onSyncChanged(bool synchronize);

};

QT_END_NAMESPACE_U1DB

#endif // U1DB_SYNCHRONIZER_H

