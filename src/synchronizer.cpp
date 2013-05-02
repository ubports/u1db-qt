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

#include <QDebug>
#include <QSqlQuery>
#include <QFile>
#include <QSqlError>
#include <QUuid>
#include <QStringList>
#include <QJsonDocument>

#include "synchronizer.h"
#include "private.h"

QT_BEGIN_NAMESPACE_U1DB

/*!
    \class Synchronizer
    \inmodule U1db
    \ingroup modules

    \brief The Synchronizer class handles synchronizing between two databases.

*/

/*

 Below this line are methods that are standards in declarative API design. They include the element instantiation, default settings for properties and get/set methods for properties. 

*/

/*!
    Instantiate a new Synchronizer with an optional \a parent, usually by declaring it as a QML item.
    
    Synchronizer elements sync two databases together, a 'source' database and a remote or local 'target' database.
    
 */
 
Synchronizer::Synchronizer(QObject *parent) :
    QObject(parent), m_source(0), m_target(0), m_resolver(0), m_synchronize(false)
{
}

/*!
    \property Synchronizer::source
 */
void Synchronizer::setSource(Database* source)
{
	
	if (m_source == source)
		return;

	if (m_source)
		QObject::disconnect(m_source, 0, this, 0);
		
	m_source = source;
		
	Q_EMIT sourceChanged(source);
}
/*!
    \property Synchronizer::target
 */
void Synchronizer::setTarget(Database* target)
{
	
	if (m_target == target)
		return;

	if (m_target)
		QObject::disconnect(m_target, 0, this, 0);
		
	m_target = target;
	Q_EMIT targetChanged(target);
}
/*!
    \property Synchronizer::resolver
 */
void Synchronizer::setResolver(Database* resolver)
{

	if (m_resolver == resolver)
		return;

	if (m_resolver)
		QObject::disconnect(m_resolver, 0, this, 0);	

	m_resolver = resolver;
	Q_EMIT resolverChanged(resolver);
}
/*!
    \property Synchronizer::synchronize
 */
void Synchronizer::setSync(bool synchronize)
{

	if (m_synchronize == synchronize)
		return;
	
	m_synchronize = synchronize;
	Q_EMIT syncChanged(synchronize);
}

/*!
    
 */
Database* Synchronizer::getSource()
{
	return m_source;
}

/*!
    
 */
Database* Synchronizer::getTarget()
{
	return m_target;
}

/*!
    
 */
Database* Synchronizer::getResolver()
{
	return m_resolver;
}

/*!
    
 */
bool Synchronizer::getSync()
{
	return m_synchronize;
}


/*

 Below this line represents the unique API methods that are not part of standard declarative API design.

*/


QT_END_NAMESPACE_U1DB

#include "moc_synchronizer.cpp"

