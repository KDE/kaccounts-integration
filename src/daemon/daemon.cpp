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
#include "kio/kioservices.h"

#include <kdebug.h>
#include <kdemacros.h>
#include <KLocalizedString>
#include <KActionCollection>
#include <KPluginFactory>

#include <Accounts/Manager>
#include <Accounts/Service>
#include <Accounts/AccountService>

K_PLUGIN_FACTORY(KScreenDaemonFactory, registerPlugin<AccountsDaemon>();)
K_EXPORT_PLUGIN(KScreenDaemonFactory("accounts", "accounts"))

AccountsDaemon::AccountsDaemon(QObject* parent, const QList< QVariant >& )
 : KDEDModule(parent)
 , m_manager(new Accounts::Manager(this))
 , m_akonadi(new AkonadiServices(this))
 , m_kio(new KIOServices(this))
{
    QMetaObject::invokeMethod(this, "startDaemon", Qt::QueuedConnection);
    connect(m_manager, SIGNAL(accountCreated(Accounts::AccountId)), SLOT(accountCreated(Accounts::AccountId)));
    connect(m_manager, SIGNAL(accountRemoved(Accounts::AccountId)), SLOT(accountRemoved(Accounts::AccountId)));
}

AccountsDaemon::~AccountsDaemon()
{
    delete m_manager;
    delete m_akonadi;
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

    QMap <QString, QString> servicesInfo;
    Q_FOREACH(const Accounts::Service &service, services) {
        servicesInfo.insert(service.name(), service.serviceType());
    }

    m_akonadi->serviceAdded(id, servicesInfo);
    m_kio->serviceAdded(id, servicesInfo);

    delete acc;
}

void AccountsDaemon::accountRemoved(const Accounts::AccountId& id)
{
    kDebug() << id;

    Accounts::Account *acc = m_manager->account(id);
    Accounts::ServiceList services = acc->enabledServices();

    QMap <QString, QString> servicesInfo;
    Q_FOREACH(const Accounts::Service &service, services) {
        servicesInfo.insert(service.name(), service.serviceType());
    }

    m_akonadi->serviceRemoved(id, servicesInfo);
    m_kio->serviceRemoved(id, servicesInfo);

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

    QMap<QString, QString> services;
    services.insert(serviceName, m_manager->service(serviceName).serviceType());

    if (!enabled) {
        m_akonadi->serviceDisabled(accId, services);
        m_kio->serviceDisabled(accId, services);
        return;
    }

    m_akonadi->serviceEnabled(accId, services);
    m_kio->serviceEnabled(accId, services);
}
