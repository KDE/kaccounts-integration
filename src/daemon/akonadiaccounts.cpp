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
    m_configName = configName;
}

bool AkonadiAccounts::hasServices(const Accounts::AccountId& accId)
{
    kDebug() << accId;
    QString key("Account_" + QString::number(accId));
    KConfigGroup account = accounts()->group(key);
    return !account.keyList().isEmpty();
}

QStringList AkonadiAccounts::services(const Accounts::AccountId& accId)
{
    kDebug() << accId;
    QString key("Account_" + QString::number(accId));
    KConfigGroup account = accounts()->group(key);
    return account.keyList();
}

void AkonadiAccounts::addService(const Accounts::AccountId& accId, const QString& serviceName, const QString& resourceId)
{
    kDebug() << accId << serviceName << resourceId;
    QString key("Account_" + QString::number(accId));

    KConfigGroup account = accounts()->group(key);
    account.writeEntry(serviceName, resourceId);
    account.sync();
}

void AkonadiAccounts::removeService(const Accounts::AccountId& accId, const QString& serviceName)
{
    kDebug() << accId << serviceName;
    QString key("Account_" + QString::number(accId));
    KConfigGroup group = accounts()->group(key);
    group.deleteEntry(serviceName);

    if (group.entryMap().isEmpty()) {
        accounts()->deleteGroup(key);
    }

    accounts()->sync();
}

QString AkonadiAccounts::resource(const Accounts::AccountId& accId, const QString& serviceName) const
{
    kDebug() << accId << serviceName;
    QString key("Account_" + QString::number(accId));

    return accounts()->group(key).readEntry(serviceName, QString());
}

QString AkonadiAccounts::resourceFromType(const Accounts::AccountId& accId, const QString& resourceType) const
{
    if (!group(accId).isValid()) {
        return QString();
    }
    QStringList resources = group(accId).entryMap().values();

    Q_FOREACH(const QString &instance, resources) {
        if (instance.startsWith(resourceType)) {
            return instance;
        }
    }

    return QString();
}

KSharedConfig::Ptr AkonadiAccounts::accounts() const
{
    Q_ASSERT(!m_configName.isEmpty());
    return KSharedConfig::openConfig(m_configName);
}

KConfigGroup AkonadiAccounts::group(const Accounts::AccountId &accId) const
{
    QString group("Account_" + QString::number(accId));
    kDebug() << accounts();
    if (!accounts()->hasGroup(group)) {
        return KConfigGroup();
    }

    return accounts()->group(group);
}