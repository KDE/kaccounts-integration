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

#include "akonadiaccounts.h"

#include <KDebug>
#include <KConfigGroup>
#include <KSharedConfig>

AkonadiAccounts::AkonadiAccounts()
{
    AkonadiAccounts(QLatin1String("accounts-akonadi"));
}

AkonadiAccounts::AkonadiAccounts(const QString& configName)
{
    m_accounts = KSharedConfig::openConfig(configName);
}

void AkonadiAccounts::addResource(const Accounts::AccountId& accId, const QString& serviceName, const QString& agentIdentifier)
{
    addService(accId, serviceName, agentIdentifier);
}

bool AkonadiAccounts::hasServices(const Accounts::AccountId& accId)
{
    kDebug() << accId;
    QString key("Account_" + QString::number(accId));
    KConfigGroup account = m_accounts->group(key);
    return !account.keyList().isEmpty();
}

QStringList AkonadiAccounts::services(const Accounts::AccountId& accId)
{
    kDebug() << accId;
    QString key("Account_" + QString::number(accId));
    KConfigGroup account = m_accounts->group(key);
    return account.keyList();
}

void AkonadiAccounts::addService(const Accounts::AccountId& accId, const QString& serviceName, const QString& resourceId)
{
    kDebug() << accId << serviceName << resourceId;
    QString key("Account_" + QString::number(accId));

    KConfigGroup account = m_accounts->group(key);
    account.writeEntry(serviceName, resourceId);
    account.sync();
}

void AkonadiAccounts::removeService(const Accounts::AccountId& accId, const QString& serviceName)
{
    kDebug() << accId << serviceName;
    QString key("Account_" + QString::number(accId));
    KConfigGroup group = m_accounts->group(key);
    group.deleteEntry(serviceName);

    if (group.entryMap().isEmpty()) {
        m_accounts->deleteGroup(key);
    }

    m_accounts->sync();
}

void AkonadiAccounts::removeResources(const Accounts::AccountId& accId, const QString& serviceName)
{
    removeService(accId, serviceName);
}

void AkonadiAccounts::removeAccount(const Accounts::AccountId& accId)
{
    kDebug() << accId;
    QString key("Account_" + QString::number(accId));
    m_accounts->deleteGroup(key);
}

QString AkonadiAccounts::resource(const Accounts::AccountId& accId, const QString& serviceName) const
{
    kDebug() << accId << serviceName;
    QString key("Account_" + QString::number(accId));

    return m_accounts->group(key).readEntry(serviceName, QString());
}

QStringList AkonadiAccounts::resources(const Accounts::AccountId& accId, const QString& serviceName) const
{
    QString key("Account_" + QString::number(accId));
    return m_accounts->group(key).readEntry(serviceName, QStringList());
}

QString AkonadiAccounts::resourceFromType(const Accounts::AccountId& accId, const QString& resourceType) const
{
    QString key("Account_" + QString::number(accId));
    QStringList resources = m_accounts->group(key).entryMap().values();

    Q_FOREACH(const QString &instance, resources) {
        if (instance.startsWith(resourceType)) {
            return instance;
        }
    }

    return QString();
}

QString AkonadiAccounts::createdResource(const Accounts::AccountId& accId, const QString& resource) const
{
    return resourceFromType(accId, resource);
}