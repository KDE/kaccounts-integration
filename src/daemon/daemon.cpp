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

#include "daemon.h"
#include "akonadi/akonadiservices.h"
#include "akonadi/jobs/createresourcejob.h"
#include "akonadi/jobs/removeresourcejob.h"
#include "akonadi/jobs/enableservicejob.h"
#include "akonadi/jobs/lookupakonadiservices.h"
#include "akonadi/jobs/removeakonadiservicesjob.h"

#include "akonadi/akonadiaccounts.h"

#include <QtCore/QTimer>

#include <kdebug.h>
#include <kdemacros.h>
#include <KLocalizedString>
#include <KActionCollection>
#include <KPluginFactory>

#include <Accounts/Manager>
#include <Accounts/Service>
#include <Accounts/AccountService>

#include <Akonadi/AgentType>
#include <Akonadi/AgentManager>

K_PLUGIN_FACTORY(KScreenDaemonFactory, registerPlugin<AccountsDaemon>();)
K_EXPORT_PLUGIN(KScreenDaemonFactory("accounts", "accounts"))

using namespace Akonadi;

AccountsDaemon::AccountsDaemon(QObject* parent, const QList< QVariant >& )
 : KDEDModule(parent)
 , m_manager(new Accounts::Manager(this))
 , m_accounts(new AkonadiAccounts())
{
    QMetaObject::invokeMethod(this, "startDaemon", Qt::QueuedConnection);
    connect(m_manager, SIGNAL(accountCreated(Accounts::AccountId)), SLOT(accountCreated(Accounts::AccountId)));
    connect(m_manager, SIGNAL(accountRemoved(Accounts::AccountId)), SLOT(accountRemoved(Accounts::AccountId)));
}

AccountsDaemon::~AccountsDaemon()
{
    delete m_manager;
    delete m_accounts;
    delete m_akonadi;
}

void AccountsDaemon::startDaemon()
{
    kDebug();
    Accounts::AccountIdList accList = m_manager->accountList();
    Q_FOREACH(const Accounts::AccountId &id, accList) {
        monitorAccount(id);
    }

    m_akonadi = new AkonadiServices();
}

void AccountsDaemon::monitorAccount(const Accounts::AccountId &id)
{
    kDebug() << id;
    Accounts::Account *acc = m_manager->account(id);
    Accounts::ServiceList services = acc->services();
    Q_FOREACH(const Accounts::Service &service, services) {
        acc->selectService(service);
    }

    connect(acc, SIGNAL(enabledChanged(QString,bool)), SLOT(enabledChanged(QString,bool)));
}

void AccountsDaemon::accountCreated(const Accounts::AccountId &id)
{
    kDebug() << id;
    monitorAccount(id);

    Accounts::Account *acc = m_manager->account(id);
    Accounts::ServiceList services = acc->enabledServices();

    QMap <QString, QString> servicesInfo;
    Q_FOREACH(const Accounts::Service &service, services) {
        servicesInfo.insert(service.name(), service.serviceType());
    }

    m_akonadi->serviceAdded(id, servicesInfo);

    delete acc;
}

void AccountsDaemon::accountRemoved(const Accounts::AccountId& id)
{
    kDebug() << id;

    Accounts::Account *acc = m_manager->account(id);
    Accounts::ServiceList services = acc->enabledServices();

    RemoveAkonadiServicesJob *job = new RemoveAkonadiServicesJob(m_accounts, this);
    job->setAccountId(id);
    job->start();

    delete acc;
}

void AccountsDaemon::enabledChanged(const QString& serviceName, bool enabled)
{
    kDebug();
    if (serviceName.isEmpty()) {
        kDebug() << "ServiceName is Empty";
        return;
    }

    Accounts::AccountId accId = qobject_cast<Accounts::Account*>(sender())->id();
    if (!enabled) {
        removeService(accId, serviceName);
        return;
    }

    QMap<QString, QString> services;
    services.insert(serviceName, m_manager->service(serviceName).serviceType());

    LookupAkonadiServices *lookup = new LookupAkonadiServices(m_accounts, this);
    lookup->setServices(services);
    lookup->setAccountId(accId);
    lookup->start();
}

void AccountsDaemon::removeService(const Accounts::AccountId& accId, const QString& serviceName)
{
    kDebug() << accId << serviceName;
    EnableServiceJob *job = new EnableServiceJob(this);
    connect(job, SIGNAL(finished(KJob*)), SLOT(disableServiceJobDone(KJob*)));
    job->setResourceId(m_accounts->resource(accId, serviceName));
    job->setServiceName(serviceName);
    job->setServiceType(m_manager->service(serviceName).serviceType(), EnableServiceJob::Disable);
    job->setAccountId(accId);
    job->start();
}

void AccountsDaemon::disableServiceJobDone(KJob* job)
{
    kDebug();
    if (job->error()) {
        kDebug() << job->errorText();
        return;
    }

    EnableServiceJob *serviceJob = qobject_cast<EnableServiceJob*>(job);
    AgentManager::self()->instance(serviceJob->resourceId()).reconfigure();
    m_accounts->removeService(serviceJob->accountId(), serviceJob->serviceName());

    if (!m_accounts->resources(serviceJob->accountId()).contains(serviceJob->resourceId())) {
        RemoveResourceJob *rJob = new RemoveResourceJob(this);
        rJob->setResourceId(serviceJob->resourceId());
        rJob->start();
    }
}