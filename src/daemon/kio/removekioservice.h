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

#ifndef REMOVE_KIOSERVICE_H
#define REMOVE_KIOSERVICE_H

#include <KJob>

#include <Accounts/Account>

class RemoveKioService : public KJob
{
    Q_OBJECT
    public:
        explicit RemoveKioService(QObject* parent = 0);

        virtual void start();

        Accounts::AccountId accountId() const;
        void setAccountId(const Accounts::AccountId &accId);

        QString serviceName() const;
        void setServiceName(const QString &serviceName);

    private Q_SLOTS:
        void removeKioService();
        void removeNetAatachFinished(KJob* job);

    private:
        Accounts::AccountId m_accountId;
        QString m_serviceName;
};

#endif //REMOVE_KIOSERVICE_H
