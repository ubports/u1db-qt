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

#include "database.h"
#include "document.h"
#include "index.h"
#include "query.h"
#include "synchronizer.h"
#include "plugin.h"
#include <qqml.h>

QT_USE_NAMESPACE_U1DB

void U1DBPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<Database>(uri, 1, 0, "Database");
    qmlRegisterType<Document>(uri, 1, 0, "Document");
    qmlRegisterType<Index>(uri, 1, 0, "Index");
    qmlRegisterType<Query>(uri, 1, 0, "Query");
    qmlRegisterType<Query>(uri, 1, 0, "Synchronizer");
}

