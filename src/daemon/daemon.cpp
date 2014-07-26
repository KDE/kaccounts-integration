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
// #include "akonadi/akonadiservices.h"
#include "kio/kioservices.h"

#include <KLocalizedString>
#include <KPluginFactory>

#include <QDebug>

#include <Accounts/Manager>
#include <Accounts/Service>
#include <Accounts/AccountService>

K_PLUGIN_FACTORY_WITH_JSON(AccountsDaemonFactory, "accounts-daemon.json", registerPlugin<AccountsDaemon>();)

AccountsDaemon::AccountsDaemon(QObject* parent, const QList< QVariant >& )
 : KDEDModule(parent)
 , m_manager(new Accounts::Manager(this))
 //, m_akonadi(new AkonadiServices(this))
 , m_kio(new KIOServices(this))
{
    QMetaObject::invokeMethod(this, "startDaemon", Qt::QueuedConnection);
    connect(m_manager, SIGNAL(accountCreated(Accounts::AccountId)), SLOT(accountCreated(Accounts::AccountId)));
    connect(m_manager, SIGNAL(accountRemoved(Accounts::AccountId)), SLOT(accountRemoved(Accounts::AccountId)));
}

AccountsDaemon::~AccountsDaemon()
{
    delete m_manager;
//     delete m_akonadi;
    delete m_kio;
}

void AccountsDaemon::startDaemon()
{
    qDebug();
    Accounts::AccountIdList accList = m_manager->accountList();
    Q_FOREACH(const Accounts::AccountId &id, accList) {
        monitorAccount(id);
    }
}

void AccountsDaemon::monitorAccount(const Accounts::AccountId &id)
{
    qDebug() << id;
    Accounts::Account *acc = m_manager->account(id);
    Accounts::ServiceList services = acc->services();
    Q_FOREACH(const Accounts::Service &service, services) {
        acc->selectService(service);
    }
    acc->selectService();

    connect(acc, SIGNAL(enabledChanged(QString,bool)), SLOT(enabledChanged(QString,bool)));
}

void AccountsDaemon::accountCreated(const Accounts::AccountId &id)
{
    qDebug() << id;
    monitorAccount(id);

    Accounts::Account *acc = m_manager->account(id);
    Accounts::ServiceList services = acc->enabledServices();

//     m_akonadi->accountCreated(id, services);
    m_kio->accountCreated(id, services);
}

void AccountsDaemon::accountRemoved(const Accounts::AccountId& id)
{
    qDebug() << id;

//     m_akonadi->accountRemoved(id);
    m_kio->accountRemoved(id);
}

void AccountsDaemon::enabledChanged(const QString& serviceName, bool enabled)
{
    qDebug();
    if (serviceName.isEmpty()) {
        qDebug() << "ServiceName is Empty";
        return;
    }

    Accounts::AccountId accId = qobject_cast<Accounts::Account*>(sender())->id();

    Accounts::Service service = m_manager->service(serviceName);
    if (!enabled) {
//         m_akonadi->serviceDisabled(accId, service);
        m_kio->serviceDisabled(accId, service);
        return;
    }

//     m_akonadi->serviceEnabled(accId, service);
    m_kio->serviceEnabled(accId, service);
}

#include "daemon.moc"