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

#include "akonadiservices.h"
#include "akonadiaccounts.h"

#include "jobs/lookupakonadiservices.h"
#include "jobs/removeakonadiservicesjob.h"
#include "jobs/enableservicejob.h"
#include "jobs/removeresourcejob.h"

#include <QDebug>
#include <AkonadiCore/AgentManager>

using namespace Akonadi;

AkonadiServices::AkonadiServices(QObject* parent)
    : KAccountsDPlugin(parent)
    , m_accounts(new AkonadiAccounts())
{

}

AkonadiServices::~AkonadiServices()
{
    delete m_accounts;
}

void AkonadiServices::onAccountCreated(const Accounts::AccountId accId, const Accounts::ServiceList &serviceList)
{
    LookupAkonadiServices *lookup = new LookupAkonadiServices(m_accounts, this);
    lookup->setServices(serviceList);
    lookup->setAccountId(accId);

    connect(lookup, &LookupAkonadiServices::finished, this, [=](KJob *job){
        if (job->error()) {
            qDebug() << "Lookup job failed" << job->errorText();
        }
    });

    lookup->start();
}

void AkonadiServices::onAccountRemoved(const Accounts::AccountId accId)
{
    if (!m_accounts->hasServices(accId)) {
        qDebug() << "No service enabled";
        return;
    }

    RemoveAkonadiServicesJob *job = new RemoveAkonadiServicesJob(m_accounts, this);
    job->setAccountId(accId);
    job->start();
}

void AkonadiServices::onServiceEnabled(const Accounts::AccountId accId, const Accounts::Service &service)
{
    Accounts::ServiceList list;
    list.append(service);
    onAccountCreated(accId, list);
}

void AkonadiServices::onServiceDisabled(const Accounts::AccountId accId, const Accounts::Service &service)
{
    if (!m_accounts->hasService(accId, service.name())) {
        qDebug() << "No service enabled";
        return;
    }

    QString serviceName = service.name();
    EnableServiceJob *job = new EnableServiceJob(this);
    connect(job, SIGNAL(finished(KJob*)), SLOT(disableServiceJobDone(KJob*)));
    job->setResourceId(m_accounts->resource(accId, serviceName));
    job->addService(service, EnableServiceJob::Disable);
    job->setAccountId(accId);
    job->start();
}

void AkonadiServices::disableServiceJobDone(KJob *job)
{
    qDebug();
    if (job->error()) {
        qDebug() << job->errorText();
        return;
    }

    EnableServiceJob *serviceJob = qobject_cast<EnableServiceJob*>(job);
    AgentManager::self()->instance(serviceJob->resourceId()).reconfigure();
    m_accounts->removeService(serviceJob->accountId(), serviceJob->services());

    if (!m_accounts->resources(serviceJob->accountId()).contains(serviceJob->resourceId())) {
        RemoveResourceJob *rJob = new RemoveResourceJob(this);
        rJob->setResourceId(serviceJob->resourceId());
        rJob->start();
    }
}