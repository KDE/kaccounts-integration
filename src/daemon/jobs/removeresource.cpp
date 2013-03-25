/*************************************************************************************
 *  Copyright (C) 2012-2013 by Alejandro Fiestas Olivares <afiestas@kde.org>         *
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

#include "removeresource.h"

#include <akonadi/agentmanager.h>

#include <KDebug>

using namespace Akonadi;

RemoveResource::RemoveResource(QObject* parent)
: KJob(parent)
{
}


RemoveResource::~RemoveResource()
{

}

void RemoveResource::start()
{
    QMetaObject::invokeMethod(this, "removeResource", Qt::QueuedConnection);
}

void RemoveResource::setAgentIdentifier(const QString& agent)
{
    m_agentIdentifier = agent;
}

void RemoveResource::removeResource()
{

    AgentInstance instance = AgentManager::self()->instance(m_agentIdentifier);
    if (instance.isValid()) {
        AgentManager::self()->removeInstance(instance);
    } else {
        kDebug() << "Agent not found";
    }

    emitResult();
}
