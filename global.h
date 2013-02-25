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

#ifndef U1DB_GLOBAL_H
#define U1DB_GLOBAL_H

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
#  define QT_BEGIN_NAMESPACE_U1DB namespace QT_NAMESPACE { namespace U1db {
#  define QT_END_NAMESPACE_U1DB } }
#  define QT_USE_NAMESPACE_U1DB using namespace QT_NAMESPACE::U1db;
#  define QT_PREPEND_NAMESPACE_U1DB(name) QT_NAMESPACE::U1db::name
#else
#  define QT_BEGIN_NAMESPACE_U1DB namespace U1db {
#  define QT_END_NAMESPACE_U1DB }
#  define QT_USE_NAMESPACE_U1DB using namespace U1db;
#  define QT_PREPEND_NAMESPACE_U1DB(name) U1db::name
#endif

// a workaround for moc - if there is a header file that doesn't use u1db
// namespace, we still force moc to do "using namespace" but the namespace have to
// be defined, so let's define an empty namespace here
QT_BEGIN_NAMESPACE_U1DB
QT_END_NAMESPACE_U1DB

#endif // U1DB_GLOBAL_H

