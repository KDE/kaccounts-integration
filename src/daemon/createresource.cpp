/*************************************************************************************
 *  Copyright (C) 2013 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#include "createresource.h"

#include <QtCore/QDebug>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>

#include <Akonadi/AgentManager>
#include <Akonadi/AgentInstanceCreateJob>

#include <kdebug.h>

using namespace Akonadi;

CreateResource::CreateResource(QObject* parent)
 : KJob(parent)
{
}

CreateResource::~CreateResource()
{

}

void CreateResource::start()
{
    qDebug() << m_accountId;
    AgentInstanceCreateJob *job = new AgentInstanceCreateJob(m_type, this);
    connect(job, SIGNAL(result(KJob*)), SLOT(resourceCreated(KJob*)));

    job->start();
}

Accounts::AccountId CreateResource::accountId() const
{
    return m_accountId;
}

void CreateResource::setAccountId(const Accounts::AccountId& accId)
{
    m_accountId = accId;
}

QString CreateResource::serviceName() const
{
    return m_serviceName;
}

void CreateResource::setServiceName(const QString& serviceName)
{
    m_serviceName = serviceName;
}

void CreateResource::setAgentType(const AgentType& type)
{
    m_type = type;
}

QString CreateResource::agentIdentifier() const
{
    return m_agentIdentifier;
}

void CreateResource::resourceCreated(KJob* job)
{
    kDebug();
    if (job->error()) {
        kDebug() << "Error creating the job";
        return;
    }

    AgentInstance agent = qobject_cast<AgentInstanceCreateJob*>( job )->instance();
    QString service = "org.freedesktop.Akonadi.Resource." + agent.identifier();
    QString path = "/org/kde/mklapetek";
    QString method = "org.kde.";
    method.append(agent.identifier());
    method.append(".MicroblogResource");

    QDBusMessage msg = QDBusMessage::createMethodCall(service, path, method, "configureByAccount");
    msg.setArguments(QList<QVariant>() << m_accountId);

    QDBusConnection::sessionBus().asyncCall(msg);

    m_agentIdentifier = agent.identifier();
    emitResult();
}
