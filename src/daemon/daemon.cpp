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
#include "src/lib/kaccountsdplugin.h"
#include <core.h>

#include <KPluginFactory>
#include <KPluginMetaData>
#include <KPluginLoader>

#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QCoreApplication>

#include <Accounts/Manager>
#include <Accounts/Service>
#include <Accounts/AccountService>

K_PLUGIN_FACTORY_WITH_JSON(AccountsDaemonFactory, "accounts.json", registerPlugin<AccountsDaemon>();)

AccountsDaemon::AccountsDaemon(QObject *parent, const QList<QVariant>&)
 : KDEDModule(parent)
{

    QMetaObject::invokeMethod(this, "startDaemon", Qt::QueuedConnection);
    connect(KAccounts::accountsManager(), &Accounts::Manager::accountCreated, this, &AccountsDaemon::accountCreated);
    connect(KAccounts::accountsManager(), &Accounts::Manager::accountRemoved, this, &AccountsDaemon::accountRemoved);

    const QVector<KPluginMetaData> data = KPluginLoader::findPlugins(QStringLiteral("kaccounts/daemonplugins"));
    for (const KPluginMetaData& metadata : data) {

        if (!metadata.isValid()) {
            qDebug() << "Invalid metadata" << metadata.name();
            continue;
        }

        KPluginLoader loader(metadata.fileName());
        KPluginFactory* factory = loader.factory();

        if (!factory) {
            qDebug() << "KPluginFactory could not load the plugin:" << metadata.pluginId() << loader.errorString();
            continue;
        }

        KAccountsDPlugin* plugin = factory->create<KAccountsDPlugin>(this, QVariantList());
        if (!plugin) {
            qDebug() << "Error loading plugin" << metadata.name() << loader.errorString();
            continue;
        }

        m_plugins << plugin;
    }
}

AccountsDaemon::~AccountsDaemon()
{
    qDeleteAll(m_plugins);
}

void AccountsDaemon::startDaemon()
{
    qDebug();
    const Accounts::AccountIdList accList = KAccounts::accountsManager()->accountList();
    for (const Accounts::AccountId &id : accList) {
        monitorAccount(id);
    }
}

void AccountsDaemon::monitorAccount(const Accounts::AccountId id)
{
    qDebug() << id;
    Accounts::Account *acc = KAccounts::accountsManager()->account(id);
    const Accounts::ServiceList services = acc->services();
    for (const Accounts::Service &service : services) {
        acc->selectService(service);
    }
    acc->selectService();

    connect(acc, &Accounts::Account::enabledChanged, this, &AccountsDaemon::enabledChanged);
}

void AccountsDaemon::accountCreated(const Accounts::AccountId id)
{
    qDebug() << id;
    monitorAccount(id);

    const Accounts::Account *acc = KAccounts::accountsManager()->account(id);
    const Accounts::ServiceList services = acc->enabledServices();

    for (KAccountsDPlugin *plugin : qAsConst(m_plugins)) {
        plugin->onAccountCreated(id, services);
    }
}

void AccountsDaemon::accountRemoved(const Accounts::AccountId id)
{
    qDebug() << id;

    for (KAccountsDPlugin *plugin : qAsConst(m_plugins)) {
        plugin->onAccountRemoved(id);
    }
}

void AccountsDaemon::enabledChanged(const QString &serviceName, bool enabled)
{
    qDebug();
    if (serviceName.isEmpty()) {
        qDebug() << "ServiceName is Empty";
        return;
    }

    const Accounts::AccountId accId = qobject_cast<Accounts::Account*>(sender())->id();

    const Accounts::Service service = KAccounts::accountsManager()->service(serviceName);
    if (!enabled) {
        for (KAccountsDPlugin *plugin : qAsConst(m_plugins)) {
            plugin->onServiceDisabled(accId, service);
        }
    } else {
        for (KAccountsDPlugin *plugin : qAsConst(m_plugins)) {
            plugin->onServiceEnabled(accId, service);
        }
    }
}

#include "daemon.moc"
