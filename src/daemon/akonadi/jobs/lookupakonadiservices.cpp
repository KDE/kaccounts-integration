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

#include "lookupakonadiservices.h"
#include "enableservicejob.h"
#include "createresourcejob.h"
#include "../akonadiaccounts.h"

#include <QDebug>
#include <AkonadiCore/AgentManager>

using namespace Akonadi;

LookupAkonadiServices::LookupAkonadiServices(AkonadiAccounts *accounts, QObject* parent)
 : AbstractAkonadiJob(parent)
 , m_accounts(accounts)
{

}

void LookupAkonadiServices::start()
{
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void LookupAkonadiServices::init()
{
    if (m_services.isEmpty()) {
        emitResult();
        return;
    }

    findResource();
}

void LookupAkonadiServices::findResource()
{
    qDebug();

    if (m_services.isEmpty()) {
        emitResult();
        return;
    }

    QString serviceName = m_services.keys().first();
    QString serviceType = m_services.take(serviceName);

    qDebug() << "Looking for: " << serviceType;
    QString resourceInstance;
    const QString accProp = QLatin1String("KAccounts");
    AgentType::List types = AgentManager::self()->types();
    Q_FOREACH(const AgentType &type, types) {
        const QVariantMap props = type.customProperties();
        if (!props.contains(accProp)) {
            continue;
        }
        const QStringList services = props[accProp].toStringList();
        if (!services.contains(serviceType)) {
            continue;
        }

        resourceInstance = m_accounts->resourceFromType(m_accountId, type.identifier());
        if (!resourceInstance.isEmpty()) {
            qDebug() << "Already created, enabling service:" << resourceInstance;
            EnableServiceJob *job = new EnableServiceJob(this);
            connect(job, SIGNAL(finished(KJob*)), SLOT(enableServiceJobDone(KJob*)));
            job->setAccountId(m_accountId);
            job->setResourceId(resourceInstance);
            job->setServiceName(serviceName);
            job->setServiceType(serviceType, EnableServiceJob::Enable);
            job->start();
            return;
        }

        qDebug() << "Creating a new resource";
        CreateResourceJob *job = new CreateResourceJob(this);
        connect(job, SIGNAL(finished(KJob*)), SLOT(createResourceJobDone(KJob*)));

        qDebug() << "Found one: " << m_accountId << type.name() << serviceName;
        job->setAccountId(m_accountId);
        job->setAgentType(type);
        job->setServiceName(serviceName);
        job->setServiceType(serviceType);
        job->start();
        return;
    }

    qDebug() << "No resource found";
}

void LookupAkonadiServices::createResourceJobDone(KJob* job)
{
    qDebug();
    CreateResourceJob *cJob = qobject_cast<CreateResourceJob*>(job);
    if (cJob->error()) {
        qDebug() << "Error creating resource for: " << cJob->serviceName();
    }

    EnableServiceJob *eJob = new EnableServiceJob(this);
    connect(eJob, SIGNAL(finished(KJob*)), SLOT(enableServiceJobDone(KJob*)));
    eJob->setAccountId(cJob->accountId());
    eJob->setResourceId(cJob->resourceId());
    eJob->setServiceType(cJob->serviceType(), EnableServiceJob::Enable);
    eJob->setServiceName(cJob->serviceName());
    eJob->start();
}

void LookupAkonadiServices::enableServiceJobDone(KJob* job)
{
    qDebug();
    EnableServiceJob *sJob = qobject_cast<EnableServiceJob*>(job);
    if (sJob->error()) {
        qDebug() << "Error enabling service for: " << sJob->serviceName() << sJob->resourceId();
    } else {
        m_accounts->addService(sJob->accountId(), sJob->serviceName(), sJob->resourceId());
        AgentManager::self()->instance(sJob->resourceId()).reconfigure();
    }

    findResource();
}

void LookupAkonadiServices::setServices(const QMap<QString, QString> &services)
{
    m_services = services;
}
