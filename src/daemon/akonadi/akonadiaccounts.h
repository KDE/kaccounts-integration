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
#include <KConfigGroup>
#include <akonadi/attribute.h>

class AkonadiAccounts
{
    public:
        AkonadiAccounts(const QString& configName = QLatin1String("accounts-akonadi"));

        bool hasServices(const Accounts::AccountId &accId);
        QStringList services(const Accounts::AccountId &accId);
        void addService(const Accounts::AccountId &accId, const QString &serviceName, const QString &resourceId);
        void removeService(const Accounts::AccountId &accId, const QString &serviceName);

        QStringList resources(const Accounts::AccountId &accId);
        QString resource(const Accounts::AccountId &accId, const QString &serviceName) const;
        QString resourceFromType(const Accounts::AccountId &accId, const QString &resourceType) const;

        void removeAccount(const Accounts::AccountId &accId);
    private:
        KSharedConfig::Ptr m_config;
        KConfigGroup group(const Accounts::AccountId &accId) const;
};

#endif //ACCOUNTS_AKONADI_DAEMON_H