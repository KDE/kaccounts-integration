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

#ifndef ACCOUNTS_DAEMON_H
#define ACCOUNTS_DAEMON_H

#include <kdedmodule.h>

#include <Accounts/Account>

namespace Accounts {
    class Manager;
};

namespace Akonadi {
    class AgentType;
};
class KJob;

class KDE_EXPORT AccountsDaemon : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Accounts")

    public:
        AccountsDaemon(QObject *parent, const QList<QVariant>&);
        virtual ~AccountsDaemon();

    public Q_SLOTS:
        void startDaemon();
        void accountCreated(const Accounts::AccountId &id);
        void resourceCreated(KJob* job);
        void enabledChanged(const QString &serviceName, bool enabled);

    private:
        void monitorAccount(const Accounts::AccountId &id);
        void findResource(const QString &serviceName);
        void createResource(const Akonadi::AgentType &type);

        Accounts::Manager* m_manager;
};

#endif /*KSCREN_DAEMON_H*/