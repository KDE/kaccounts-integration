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

#ifndef CREATE_RESOURCE_H
#define CREATE_RESOURCE_H

#include <Accounts/Account>

#include <Akonadi/AgentType>
#include <Akonadi/AgentInstance>

#include <kjob.h>

class QDBusPendingCallWatcher;
class CreateResource : public KJob
{
    Q_OBJECT

    public:
        explicit CreateResource(QObject* parent = 0);
        virtual ~CreateResource();

        virtual void start();

        Accounts::AccountId accountId() const;
        void setAccountId(const Accounts::AccountId &accId);

        QString serviceName() const;
        void setServiceName(const QString &serviceName);

        void setAgentType(const Akonadi::AgentType &type);

        QString agentIdentifier() const;

    private Q_SLOTS:
        void resourceCreated(KJob* job);
        void setAccountDone(KJob* job);

    private:
        QString m_serviceName;
        QString m_agentIdentifier;
        Akonadi::AgentType m_type;
        Akonadi::AgentInstance m_agent;
        Accounts::AccountId m_accountId;
};

#endif //CREATE_RESOURCE_H