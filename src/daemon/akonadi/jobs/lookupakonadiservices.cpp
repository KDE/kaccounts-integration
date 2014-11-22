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

    QHash<QString, Accounts::Service> servicesHash;

    Q_FOREACH (const Accounts::Service &service, m_services) {
        servicesHash.insert(service.name(), service);
    }

    QSet<QString> services = servicesHash.keys().toSet();
    QString resourceInstance;
    const QString accProp = QLatin1String("KAccounts");
    AgentType::List types = AgentManager::self()->types();

    Q_FOREACH(const AgentType &type, types) {
        const QVariantMap props = type.customProperties();
        if (!props.contains(accProp)) {
            continue;
        }
        const QStringList resourceServices = props[accProp].toStringList();

        const QSet<QString> servicesToEnable = resourceServices.toSet().intersect(services);

        if (servicesToEnable.isEmpty()) {
            continue;
        }

        resourceInstance = m_accounts->resourceFromType(m_accountId, type.identifier());
        if (!resourceInstance.isEmpty()) {
            qDebug() << "Already created, enabling service:" << resourceInstance;
            EnableServiceJob *job = new EnableServiceJob(this);
            connect(job, SIGNAL(finished(KJob*)), SLOT(enableServiceJobDone(KJob*)));
            job->setAccountId(m_accountId);
            job->setResourceId(resourceInstance);

            Q_FOREACH (const QString &service, servicesToEnable) {
                job->addService(servicesHash.value(service), EnableServiceJob::Enable);
            }

            job->start();
            return;
        }

        qDebug() << "Creating a new resource";
        CreateResourceJob *job = new CreateResourceJob(this);
        connect(job, SIGNAL(finished(KJob*)), SLOT(createResourceJobDone(KJob*)));

        qDebug() << "Found one: " << m_accountId << type.name();
        job->setAccountId(m_accountId);
        job->setAgentType(type);
        Q_FOREACH (const QString &service, servicesToEnable) {
            job->addService(servicesHash.value(service));
        }
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
        qDebug() << "Error creating resource";
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    EnableServiceJob *servicesJob = new EnableServiceJob(this);
    connect(servicesJob, SIGNAL(finished(KJob*)), SLOT(enableServiceJobDone(KJob*)));
    servicesJob->setAccountId(cJob->accountId());
    servicesJob->setResourceId(cJob->resourceId());
    Q_FOREACH (const Accounts::Service &service, cJob->services()) {
        servicesJob->addService(service, EnableServiceJob::Enable);
    }
    servicesJob->start();
}

void LookupAkonadiServices::enableServiceJobDone(KJob* job)
{
    qDebug();
    EnableServiceJob *servicesJob = qobject_cast<EnableServiceJob*>(job);
    if (servicesJob->error()) {
        qDebug() << "Error enabling service for: " << servicesJob->resourceId();
    } else {
        m_accounts->addService(servicesJob->accountId(), servicesJob->services(), servicesJob->resourceId());
        AgentManager::self()->instance(servicesJob->resourceId()).reconfigure();
    }

}

void LookupAkonadiServices::setServices(const Accounts::ServiceList &services)
{
    m_services = services;
}
