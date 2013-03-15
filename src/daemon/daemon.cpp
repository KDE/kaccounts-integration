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
#include "createresource.h"
#include "removeresource.h"
#include "akonadiaccounts.h"

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
    QMetaObject::invokeMethod(this, "startDaemon");
    connect(m_manager, SIGNAL(accountCreated(Accounts::AccountId)), SLOT(accountCreated(Accounts::AccountId)));
    connect(m_manager, SIGNAL(accountRemoved(Accounts::AccountId)), SLOT(accountRemoved(Accounts::AccountId)));
}

AccountsDaemon::~AccountsDaemon()
{
    delete m_manager;
    delete m_accounts;
}

void AccountsDaemon::startDaemon()
{
    kDebug();
    Accounts::AccountIdList accList = m_manager->accountList();
    Q_FOREACH(const Accounts::AccountId &id, accList) {
        monitorAccount(id);
    }
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

    Q_FOREACH(const Accounts::Service &service, services) {
        findResource(service.name(), acc->id());
    }
    delete acc;
}

void AccountsDaemon::accountRemoved(const Accounts::AccountId& id)
{
    kDebug() << id;

    Accounts::Account *acc = m_manager->account(id);
    Accounts::ServiceList services = acc->enabledServices();

    Q_FOREACH(const Accounts::Service &service, services) {
        removeService(acc->id(), service.name());
    }

    m_accounts->removeAccount(acc->id());
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

    findResource(serviceName, accId);
}

void AccountsDaemon::resourceCreated(KJob* job)
{
    kDebug();
    CreateResource *cJob = qobject_cast<CreateResource*>(job);
    m_accounts->addResource(cJob->accountId(), cJob->serviceName(), cJob->agentIdentifier());
}

void AccountsDaemon::findResource(const QString &serviceName, const Accounts::AccountId &id)
{
    kDebug() << serviceName;
    QString mime = "text/x-vnd.accounts.";
    mime.append(serviceName);

    AgentType::List types = AgentManager::self()->types();
    Q_FOREACH(const AgentType &type, types) {
        if (!type.mimeTypes().contains(mime)) {
            continue;
        }

        CreateResource *job = new CreateResource(this);
        connect(job, SIGNAL(finished(KJob*)), SLOT(resourceCreated(KJob*)));

        job->setAccountId(id);
        job->setAgentType(type);
        job->setServiceName(serviceName);
        job->start();
    }
}

void AccountsDaemon::removeService(const Accounts::AccountId& accId, const QString& serviceName)
{
    kDebug() << accId << serviceName;
    QStringList resources = m_accounts->resources(accId, serviceName);
    kDebug() << resources;
    RemoveResource* removeJob = 0;
    Q_FOREACH(const QString &agent, resources) {
        removeJob = new RemoveResource(this);
        removeJob->setAgentIdentifier(agent);
        removeJob->start();
    }
    m_accounts->removeResources(accId, serviceName);
}