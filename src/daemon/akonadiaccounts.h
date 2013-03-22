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

#ifndef ACCOUNTS_AKONADI_DAEMON_H
#define ACCOUNTS_AKONADI_DAEMON_H

#include <Accounts/Account>
#include <KSharedConfig>

class AkonadiAccounts
{
    public:
        AkonadiAccounts();

        void addResource(const Accounts::AccountId &accId, const QString &serviceName, const QString &agentIdentifier);
        void removeResources(const Accounts::AccountId &accId, const QString &serviceName);
        void removeAccount(const Accounts::AccountId &accId);

        QStringList resources(const Accounts::AccountId &accId, const QString &serviceName) const;
        QString createdResource(const Accounts::AccountId& accId, const QString& resource) const;

    private:
        KSharedConfig::Ptr m_accounts;
};

#endif //ACCOUNTS_AKONADI_DAEMON_H