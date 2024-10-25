/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kded_accounts.h"
#include "debug.h"
#include "kaccountsdplugin.h"

#include <core.h>

#include <KPluginFactory>
#include <KPluginMetaData>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>

#include <Accounts/AccountService>
#include <Accounts/Manager>
#include <Accounts/Service>

K_PLUGIN_CLASS_WITH_JSON(KDEDAccounts, "kded_accounts.json")

KDEDAccounts::KDEDAccounts(QObject *parent, const QList<QVariant> &)
    : KDEDModule(parent)
{
    QMetaObject::invokeMethod(this, "startDaemon", Qt::QueuedConnection);
    connect(KAccounts::accountsManager(), &Accounts::Manager::accountCreated, this, &KDEDAccounts::accountCreated);
    connect(KAccounts::accountsManager(), &Accounts::Manager::accountRemoved, this, &KDEDAccounts::accountRemoved);

    const QVector<KPluginMetaData> data = KPluginMetaData::findPlugins(QStringLiteral("kaccounts/daemonplugins"));
    for (const KPluginMetaData &metadata : data) {
        if (!metadata.isValid()) {
            qCDebug(KACCOUNTS_KDED_LOG) << "Invalid metadata" << metadata.name();
            continue;
        }

        const auto result = KPluginFactory::instantiatePlugin<KAccounts::KAccountsDPlugin>(metadata, this, {});

        if (!result) {
            qCDebug(KACCOUNTS_KDED_LOG) << "Error loading plugin" << metadata.name() << result.errorString;
            continue;
        }

        m_plugins << result.plugin;
    }
}

KDEDAccounts::~KDEDAccounts()
{
    qDeleteAll(m_plugins);
}

void KDEDAccounts::startDaemon()
{
    qCDebug(KACCOUNTS_KDED_LOG);
    const Accounts::AccountIdList accList = KAccounts::accountsManager()->accountList();
    for (const Accounts::AccountId &id : accList) {
        monitorAccount(id);
    }
}

void KDEDAccounts::monitorAccount(const Accounts::AccountId id)
{
    qCDebug(KACCOUNTS_KDED_LOG) << id;
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
    qCDebug(KACCOUNTS_KDED_LOG) << id;
    monitorAccount(id);

    const Accounts::Account *acc = KAccounts::accountsManager()->account(id);
    const Accounts::ServiceList services = acc->enabledServices();

    for (KAccounts::KAccountsDPlugin *plugin : std::as_const(m_plugins)) {
        plugin->onAccountCreated(id, services);
    }
}

void KDEDAccounts::accountRemoved(const Accounts::AccountId id)
{
    qCDebug(KACCOUNTS_KDED_LOG) << id;

    for (KAccounts::KAccountsDPlugin *plugin : std::as_const(m_plugins)) {
        plugin->onAccountRemoved(id);
    }
}

void KDEDAccounts::enabledChanged(const QString &serviceName, bool enabled)
{
    qCDebug(KACCOUNTS_KDED_LOG);
    if (serviceName.isEmpty()) {
        qCDebug(KACCOUNTS_KDED_LOG) << "ServiceName is Empty";
        return;
    }

    const Accounts::AccountId accId = qobject_cast<Accounts::Account *>(sender())->id();

    const Accounts::Service service = KAccounts::accountsManager()->service(serviceName);
    if (!enabled) {
        for (KAccounts::KAccountsDPlugin *plugin : std::as_const(m_plugins)) {
            plugin->onServiceDisabled(accId, service);
        }
    } else {
        for (KAccounts::KAccountsDPlugin *plugin : std::as_const(m_plugins)) {
            plugin->onServiceEnabled(accId, service);
        }
    }
}

#include "kded_accounts.moc"
