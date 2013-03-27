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

#ifndef ABSTRACT_AKONADI_JOB_H
#define ABSTRACT_AKONADI_JOB_H

#include <Accounts/Account>

#include <KJob>
class AbstractAkonadiJob : public KJob
{
    Q_OBJECT
    public:
        explicit AbstractAkonadiJob(QObject* parent = 0);

        QString resourceId() const;
        void setResourceId(const QString &resourceId);

        QString serviceName() const;
        void setServiceName(const QString &serviceName);

        QString serviceType() const;
        void setServiceType(const QString &serviceType);

        QString interface() const;
        void setInterface(const QString &interface);

        Accounts::AccountId accountId() const;
        void setAccountId(const Accounts::AccountId &accountId);

    protected:
        QString m_resourceId;
        QString m_serviceName;
        QString m_serviceType;
        QString m_interface;
        Accounts::AccountId m_accountId;
};

#endif