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

#include <QtCore/QTimer>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>

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
#include <Akonadi/AgentInstanceCreateJob>

K_PLUGIN_FACTORY(KScreenDaemonFactory, registerPlugin<AccountsDaemon>();)
K_EXPORT_PLUGIN(KScreenDaemonFactory("accounts", "accounts"))

using namespace Akonadi;

AccountsDaemon::AccountsDaemon(QObject* parent, const QList< QVariant >& )
 : KDEDModule(parent)
 , m_manager(new Accounts::Manager(this))
{
    QMetaObject::InvokeMetaMethod(this, "startDaemon");
}

AccountsDaemon::~AccountsDaemon()
{
    delete m_manager;
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
    connect(m_manager, SIGNAL(accountCreated(Accounts::AccountId)), SLOT(accountCreated(Accounts::AccountId)));
}

void AccountsDaemon::accountCreated(const Accounts::AccountId &id)
{
    kDebug() << id;
    monitorAccount(id);

    Accounts::Account *acc = m_manager->account(id);
    Accounts::ServiceList services = acc->enabledServices();

    Q_FOREACH(const Accounts::Service &service, services) {
        findResource(service.name());
    }
    delete acc;
}

void AccountsDaemon::enabledChanged(const QString& serviceName, bool enabled)
{
    kDebug();
    findResource(serviceName);
}

void AccountsDaemon::findResource(const QString &serviceName)
{
    kDebug() << serviceName;
    QString mime = "text/x-vnd.accounts.";
    mime.append(serviceName);

    AgentType::List types = AgentManager::self()->types();
    Q_FOREACH(const AgentType &type, types) {
        if (!type.mimeTypes().contains(mime)) {
            continue;
        }

        createResource(type);
    }
}

void AccountsDaemon::createResource(const AgentType& type)
{
    AgentInstanceCreateJob *job = new AgentInstanceCreateJob(type, this);
    connect(job, SIGNAL(result(KJob*)), SLOT(resourceCreated(KJob*)));
    job->start();
}

void AccountsDaemon::resourceCreated(KJob* job)
{
    kDebug();
    if (job->error()) {
        kDebug() << "Error creating the job";
        return;
    }

    kDebug();
    AgentInstance agent = qobject_cast<AgentInstanceCreateJob*>( job )->instance();
    QString service = "org.freedesktop.Akonadi.Resource." + agent.identifier();
    QString path = "/org/kde/mklapetek";
    QString method = "org.kde.";
    method.append(agent.identifier());
    method.append(".MicroblogResource");

    QDBusMessage msg = QDBusMessage::createMethodCall(service, path, method, "configureByAccount");

    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(msg);

}