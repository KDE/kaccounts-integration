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

#ifndef CREATE_ACCOUNT_JOB_H
#define CREATE_ACCOUNT_JOB_H

#include <kjob.h>

namespace Accounts
{
    class Account;
    class Manager;
};
namespace SignOn
{
    class Error;
    class Identity;
    class SessionData;
    class IdentityInfo;
};
class CreateAccount : public KJob
{
    Q_OBJECT
    public:
        explicit CreateAccount(const QString &providerName, QObject* parent = 0);

        virtual void start();

    private Q_SLOTS:
        void error(const SignOn::Error &error);
        void response(const SignOn::SessionData &data);
        void info(const SignOn::IdentityInfo &info);

    private:
        QString m_providerName;
        Accounts::Manager *m_manager;
        Accounts::Account *m_account;
        SignOn::Identity *m_identity;
};
#endif //CREATE_ACCOUNT_JOB_H