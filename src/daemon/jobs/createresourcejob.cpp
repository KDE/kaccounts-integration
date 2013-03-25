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

#include "createresourcejob.h"
#include "changesettingsjob.h"

#include <QtCore/QDebug>
#include <QDBusConnection>
#include <QDBusMessage>

#include <Akonadi/AgentManager>
#include <Akonadi/AgentInstanceCreateJob>

#include <kdebug.h>

using namespace Akonadi;

CreateResourceJob::CreateResourceJob(QObject* parent)
 : KJob(parent)
{
}

CreateResourceJob::~CreateResourceJob()
{

}

void CreateResourceJob::start()
{
    qDebug() << m_accountId;
    AgentInstanceCreateJob *job = new AgentInstanceCreateJob(m_type, this);
    connect(job, SIGNAL(result(KJob*)), SLOT(resourceCreated(KJob*)));

    job->start();
}

Accounts::AccountId CreateResourceJob::accountId() const
{
    return m_accountId;
}

void CreateResourceJob::setAccountId(const Accounts::AccountId& accId)
{
    m_accountId = accId;
}

QString CreateResourceJob::serviceName() const
{
    return m_serviceName;
}

void CreateResourceJob::setServiceName(const QString& serviceName)
{
    m_serviceName = serviceName;
}

void CreateResourceJob::setAgentType(const AgentType& type)
{
    m_type = type;
}

QString CreateResourceJob::agentIdentifier() const
{
    return m_agent.identifier();
}

void CreateResourceJob::resourceCreated(KJob* job)
{
    kDebug();
    if (job->error()) {
        kDebug() << "Error creating the job";
        return;
    }

    m_agent = qobject_cast<AgentInstanceCreateJob*>( job )->instance();
    ChangeSettingsJob *setAccJob = new ChangeSettingsJob(this);
    connect(setAccJob, SIGNAL(finished(KJob*)), SLOT(setAccountDone(KJob*)));

    setAccJob->setResourceId(m_agent.identifier());
    setAccJob->setAccountId(m_accountId);
    setAccJob->setSetting("setAccountId", m_accountId);
    setAccJob->start();
}

void CreateResourceJob::setAccountDone(KJob* job)
{
    kDebug();
    m_agent.reconfigure();
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
    }

    emitResult();
}