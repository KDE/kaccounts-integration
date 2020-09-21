/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kded_accounts.h"
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

K_PLUGIN_CLASS_WITH_JSON(KDEDAccounts, "kded_accounts.json")

KDEDAccounts::KDEDAccounts(QObject *parent, const QList<QVariant>&)
 : KDEDModule(parent)
{

    QMetaObject::invokeMethod(this, "startDaemon", Qt::QueuedConnection);
    connect(KAccounts::accountsManager(), &Accounts::Manager::accountCreated, this, &KDEDAccounts::accountCreated);
    connect(KAccounts::accountsManager(), &Accounts::Manager::accountRemoved, this, &KDEDAccounts::accountRemoved);

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

KDEDAccounts::~KDEDAccounts()
{
    qDeleteAll(m_plugins);
}

void KDEDAccounts::startDaemon()
{
    qDebug();
    const Accounts::AccountIdList accList = KAccounts::accountsManager()->accountList();
    for (const Accounts::AccountId &id : accList) {
        monitorAccount(id);
    }
}

void KDEDAccounts::monitorAccount(const Accounts::AccountId id)
{
    qDebug() << id;
    Accounts::Account *acc = KAccounts::accountsManager()->account(id);
    const Accounts::ServiceList services = acc->services();
    for (const Accounts::Service &service : services) {
        acc->selectService(service);
    }
    acc->selectService();

    connect(acc, &Accounts::Account::enabledChanged, this, &KDEDAccounts::enabledChanged);
}

void KDEDAccounts::accountCreated(const Accounts::AccountId id)
{
    qDebug() << id;
    monitorAccount(id);

    const Accounts::Account *acc = KAccounts::accountsManager()->account(id);
    const Accounts::ServiceList services = acc->enabledServices();

    for (KAccountsDPlugin *plugin : qAsConst(m_plugins)) {
        plugin->onAccountCreated(id, services);
    }
}

void KDEDAccounts::accountRemoved(const Accounts::AccountId id)
{
    qDebug() << id;

    for (KAccountsDPlugin *plugin : qAsConst(m_plugins)) {
        plugin->onAccountRemoved(id);
    }
}

void KDEDAccounts::enabledChanged(const QString &serviceName, bool enabled)
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

#include "kded_accounts.moc"
