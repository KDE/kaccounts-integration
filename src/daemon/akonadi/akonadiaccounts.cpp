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

#include <QDebug>
#include <KConfigGroup>
#include <KSharedConfig>

AkonadiAccounts::AkonadiAccounts(const QString& configName)
{
    m_config = KSharedConfig::openConfig(configName);
}

bool AkonadiAccounts::hasService(const Accounts::AccountId accId, const QString& serviceName)
{
    qDebug() << accId;
    KConfigGroup account = group(accId);
    return account.keyList().contains(serviceName);
}

bool AkonadiAccounts::hasServices(const Accounts::AccountId accId)
{
    qDebug() << accId;
    KConfigGroup account = group(accId);
    return !account.keyList().isEmpty();
}

QStringList AkonadiAccounts::services(const Accounts::AccountId accId)
{
    qDebug() << accId;
    KConfigGroup account = group(accId);

    return account.keyList();
}

void AkonadiAccounts::addService(const Accounts::AccountId accId, const QString& serviceName, const QString& resourceId)
{
    addService(accId, QStringList() << serviceName, resourceId);
}

void AkonadiAccounts::addService(const Accounts::AccountId accId, const QStringList& serviceNames, const QString& resourceId)
{
    qDebug() << accId << serviceNames << resourceId;

    KConfigGroup account = group(accId);

    Q_FOREACH (const QString &service, serviceNames) {
        account.writeEntry(service, resourceId);
    }
    account.sync();
}

void AkonadiAccounts::addService(const Accounts::AccountId accId, const Accounts::ServiceList& services, const QString& resourceId)
{
    QStringList servicesToStore;

    Q_FOREACH (const Accounts::Service &service, services) {
        servicesToStore << service.name();
    }

    addService(accId, servicesToStore, resourceId);
}

void AkonadiAccounts::removeService(const Accounts::AccountId accId, const QString& serviceName)
{
    removeService(accId, QStringList() << serviceName);
}

void AkonadiAccounts::removeService(const Accounts::AccountId accId, const QStringList& serviceNames)
{
    qDebug() << accId << serviceNames;

    KConfigGroup account = group(accId);
    Q_FOREACH (const QString &service, serviceNames) {
        account.deleteEntry(service);
    }

    QString key("Account_" + QString::number(accId));
    if (account.entryMap().isEmpty()) {
        m_config->deleteGroup(key);
    }

    m_config->sync();

}

void AkonadiAccounts::removeService(const Accounts::AccountId accId, const Accounts::ServiceList &services)
{
    QStringList servicesToRemove;

    Q_FOREACH (const Accounts::Service &service, services) {
        servicesToRemove << service.name();
    }

    removeService(accId, services);
}

QStringList AkonadiAccounts::resources(const Accounts::AccountId accId)
{
    qDebug() << accId;
    KConfigGroup account = group(accId);

    QStringList cleaned;
    QStringList duplicated = account.entryMap().values();
    Q_FOREACH(const QString &resource, duplicated) {
        if (cleaned.contains(resource)) {
            continue;
        }

        cleaned.append(resource);
    }

    return cleaned;
}

QString AkonadiAccounts::resource(const Accounts::AccountId accId, const QString& serviceName) const
{
    qDebug() << accId << serviceName;

    KConfigGroup account = group(accId);
    return account.readEntry(serviceName, QString());
}

QString AkonadiAccounts::resourceFromType(const Accounts::AccountId accId, const QString& resourceType) const
{
    qDebug() << accId << resourceType;
    KConfigGroup cGroup = group(accId);
    QStringList resources = cGroup.entryMap().values();
    qDebug() << resources;
    Q_FOREACH(const QString &instance, resources) {
        if (instance.startsWith(resourceType)) {
            return instance;
        }
    }

    return QString();
}

void AkonadiAccounts::removeAccount(const Accounts::AccountId accId)
{
    qDebug();
    QString key("Account_" + QString::number(accId));
    m_config->deleteGroup(key);
    m_config->sync();
}

KConfigGroup AkonadiAccounts::group(const Accounts::AccountId accId) const
{
    QString group("Account_" + QString::number(accId));

    return m_config->group(group);
}