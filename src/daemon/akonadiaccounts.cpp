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
    m_accounts = KSharedConfig::openConfig("accounts-akonadi");
}

void AkonadiAccounts::addResource(const Accounts::AccountId& accId, const QString& serviceName, const QString& agentIdentifier)
{
    kDebug() << accId << serviceName << agentIdentifier;
    QString key("Account_" + QString::number(accId));

    KConfigGroup account = m_accounts->group(key);
    QVariantList resources = account.readEntry(serviceName, QVariantList());
    resources.append(agentIdentifier);

    account.writeEntry(serviceName, resources);
    account.sync();
}

void AkonadiAccounts::removeResources(const Accounts::AccountId& accId, const QString& serviceName)
{
    kDebug() << accId << serviceName;
    QString key("Account_" + QString::number(accId));
    m_accounts->group(key).deleteEntry(serviceName);
    m_accounts->group(key).sync();
}