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

#ifndef KIO_SERVICES_H
#define KIO_SERVICES_H

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QObject>

#include <Accounts/Account>
#include <Accounts/Service>

class KJob;
class AkonadiAccounts;
class KIOServices : public QObject
{
    Q_OBJECT
    public:
        explicit KIOServices(QObject* parent = 0);

        void accountCreated(const Accounts::AccountId& accId, const Accounts::ServiceList &serviceList);
        void accountRemoved(const Accounts::AccountId& accId);
        void serviceEnabled(const Accounts::AccountId& accId, const Accounts::Service &service);
        void serviceDisabled(const Accounts::AccountId& accId, const Accounts::Service &service);

    private:
        void enableService(const Accounts::AccountId& accId, const Accounts::Service &service);
        void disableService(const Accounts::AccountId& accId, const QString &serviceName);
        bool isEnabled(const Accounts::AccountId& accId, const QString &serviceName);
};

#endif //KIO_SERVICES_H