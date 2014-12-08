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
#include "kio/kioservices.h"
#include "src/lib/kaccountsdplugin.h"
#include <core.h>

#include <KLocalizedString>
#include <KPluginFactory>
#include <KSharedConfig>
#include <KConfigGroup>

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
    connect(KAccounts::accountsManager(), SIGNAL(accountCreated(Accounts::AccountId)), SLOT(accountCreated(Accounts::AccountId)));
    connect(KAccounts::accountsManager(), SIGNAL(accountRemoved(Accounts::AccountId)), SLOT(accountRemoved(Accounts::AccountId)));

    QStringList pluginPaths;

    QStringList paths = QCoreApplication::libraryPaths();
    Q_FOREACH (const QString &libraryPath, paths) {
        QString path(libraryPath + QStringLiteral("/kaccounts/daemonplugins"));
        QDir dir(path);

        if (!dir.exists()) {
            continue;
        }

        QStringList dirEntries = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

        Q_FOREACH(const QString &file, dirEntries) {
            pluginPaths.append(path + '/' + file);
        }
    }

    Q_FOREACH (const QString &pluginPath, pluginPaths) {
        QPluginLoader loader(pluginPath);

        if (!loader.load()) {
            qWarning() << "Could not create Extractor: " << pluginPath;
            qWarning() << loader.errorString();
            continue;
        }

        QObject *obj = loader.instance();
        if (obj) {
            KAccountsDPlugin *plugin = qobject_cast<KAccountsDPlugin*>(obj);
            if (!plugin) {
                qDebug() << "Plugin could not be converted to an ExtractorPlugin";
                qDebug() << pluginPath;
                continue;
            }
            qDebug() << "Loaded KAccounts plugin" << pluginPath;
            m_plugins << plugin;
        } else {
            qDebug() << "Plugin could not creaate instance" << pluginPath;
        }
    }

    m_plugins << new KIOServices(this);
}

AccountsDaemon::~AccountsDaemon()
{
    qDeleteAll(m_plugins);
}

void AccountsDaemon::startDaemon()
{
    qDebug();
    Accounts::AccountIdList accList = KAccounts::accountsManager()->accountList();
    Q_FOREACH(const Accounts::AccountId &id, accList) {
        monitorAccount(id);
    }
}

void AccountsDaemon::monitorAccount(const Accounts::AccountId id)
{
    qDebug() << id;
    Accounts::Account *acc = KAccounts::accountsManager()->account(id);
    Accounts::ServiceList services = acc->services();
    Q_FOREACH(const Accounts::Service &service, services) {
        acc->selectService(service);
    }
    acc->selectService();

    connect(acc, SIGNAL(enabledChanged(QString,bool)), SLOT(enabledChanged(QString,bool)));
}

void AccountsDaemon::accountCreated(const Accounts::AccountId id)
{
    qDebug() << id;
    monitorAccount(id);

    Accounts::Account *acc = KAccounts::accountsManager()->account(id);
    Accounts::ServiceList services = acc->enabledServices();

    Q_FOREACH(KAccountsDPlugin *plugin, m_plugins) {
        plugin->onAccountCreated(id, services);
    }
}

void AccountsDaemon::accountRemoved(const Accounts::AccountId id)
{
    qDebug() << id;

    Q_FOREACH(KAccountsDPlugin *plugin, m_plugins) {
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

    Accounts::AccountId accId = qobject_cast<Accounts::Account*>(sender())->id();

    Accounts::Service service = KAccounts::accountsManager()->service(serviceName);
    if (!enabled) {
        Q_FOREACH(KAccountsDPlugin *plugin, m_plugins) {
            plugin->onServiceDisabled(accId, service);
        }
    } else {
        Q_FOREACH(KAccountsDPlugin *plugin, m_plugins) {
            plugin->onServiceEnabled(accId, service);
        }
    }
}

#include "daemon.moc"
