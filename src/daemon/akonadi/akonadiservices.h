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

#ifndef AKONADI_SERVICES_H
#define AKONADI_SERVICES_H

#include <QtCore/QObject>

#include <Accounts/Account>
#include <Accounts/Service>

#include <kaccountsdplugin.h>

class KJob;
class AkonadiAccounts;
class AkonadiServices : public KAccountsDPlugin
{
    Q_OBJECT

public:
    explicit AkonadiServices(QObject *parent = 0);
    virtual ~AkonadiServices();

public Q_SLOTS:
    void onAccountCreated(const Accounts::AccountId accId, const Accounts::ServiceList &serviceList);
    void onAccountRemoved(const Accounts::AccountId accId);
    void onServiceEnabled(const Accounts::AccountId accId, const Accounts::Service &service);
    void onServiceDisabled(const Accounts::AccountId accId, const Accounts::Service &service);

private Q_SLOTS:
    void disableServiceJobDone(KJob *job);

private:
    AkonadiAccounts* m_accounts;
};

#endif //AKONADI_SERVICES_H