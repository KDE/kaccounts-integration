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

#ifndef CREATE_KIOSERVICE_H
#define CREATE_KIOSERVICE_H

#include <KJob>

#include <Accounts/Account>

namespace Accounts {
    class Manager;
};

namespace KWallet {
    class Wallet;
};

class CreateKioService : public KJob
{
    Q_OBJECT
    public:
        explicit CreateKioService(QObject* parent = 0);
        virtual ~CreateKioService();

        virtual void start();

        Accounts::AccountId accountId() const;
        void setAccountId(const Accounts::AccountId &accId);

        QString serviceName() const;
        void setServiceName(const QString &serviceName);

        QString serviceType() const;
        void setServiceType(const QString &serviceType);

    private Q_SLOTS:
        void createKioService();
        void gotCredentials(KJob *job);
        void netAttachCreated(KJob *job);

    private:
        void createDesktopFile();

        Accounts::Manager *m_manager;
        Accounts::Account *m_account;
        Accounts::AccountId m_accountId;
        QString m_serviceName;
        QString m_serviceType;
};

#endif //CREATE_KIOSERVICE_H
