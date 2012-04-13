/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include "removeakonadiresource.h"

#include <QDebug>

#include <akonadi/agenttype.h>
#include <akonadi/agentmanager.h>
#include <akonadi/agentinstancecreatejob.h>

using namespace Akonadi;

RemoveAkonadiResource::RemoveAkonadiResource(const QString &name, KConfigGroup& group, QObject* parent)
: KJob(parent)
, m_id(name)
, m_config(group)
{
    setObjectName(m_config.name());
}


RemoveAkonadiResource::~RemoveAkonadiResource()
{

}

void RemoveAkonadiResource::start()
{
    QMetaObject::invokeMethod(this, "removeResource", Qt::QueuedConnection);
}

void RemoveAkonadiResource::removeResource()
{
    m_config.sync();

    qDebug() << m_config.groupList();
    QString id = m_config.group("private").readEntry(m_id);
    id.remove("org.freedesktop.Akonadi.Resource.");
    qDebug() << "REMOVE: " << id;
    AgentInstance instance = AgentManager::self()->instance(id);

    AgentManager::self()->removeInstance(instance);

    emitResult();
}


QString RemoveAkonadiResource::id() const
{
    return m_id;
}

KConfigGroup RemoveAkonadiResource::config()
{
    return m_config;
}
