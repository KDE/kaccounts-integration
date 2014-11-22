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

#include <QDebug>
#include <QDBusConnection>
#include <QDBusMessage>

#include <AkonadiCore/AgentManager>
#include <AkonadiCore/AgentInstanceCreateJob>

using namespace Akonadi;

CreateResourceJob::CreateResourceJob(QObject* parent)
 : AbstractAkonadiJob(parent)
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

void CreateResourceJob::setAgentType(const AgentType& type)
{
    m_type = type;
}

void CreateResourceJob::resourceCreated(KJob* job)
{
    qDebug();
    if (job->error()) {
        qDebug() << "Error creating the job";
        return;
    }

    m_agent = qobject_cast<AgentInstanceCreateJob*>( job )->instance();
    m_resourceId = m_agent.identifier();
    ChangeSettingsJob *setAccJob = new ChangeSettingsJob(this);
    connect(setAccJob, SIGNAL(finished(KJob*)), SLOT(setAccountDone(KJob*)));

    setAccJob->setResourceId(m_agent.identifier());
    setAccJob->setAccountId(m_accountId);
    setAccJob->setSetting("setAccountId", m_accountId);
    setAccJob->start();
}

void CreateResourceJob::setAccountDone(KJob* job)
{
    qDebug();
    m_agent.reconfigure();
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
    }

    emitResult();
}