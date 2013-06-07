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

#ifndef GET_CREDENTIALS_JOB_H
#define GET_CREDENTIALS_JOB_H

#include <kjob.h>

#include <Accounts/Account>
#include <Accounts/AuthData>
#include <SignOn/SessionData>

namespace Accounts {
    class Manager;
};
namespace SignOn {
    class Error;
};

class GetCredentialsJob : public KJob
{
    Q_OBJECT
public:
    explicit GetCredentialsJob(const Accounts::AccountId &id, QObject* parent = 0);
    virtual void start();

    void setServiceType(const QString &serviceType);

    QVariantMap credentialsData() const;
    Accounts::AccountId accountId() const;

private Q_SLOTS:
    void getCredentials();
    void sessionResponse(const SignOn::SessionData &data);
    void sessionError(const SignOn::Error &error);

private:
    QString m_serviceType;
    Accounts::AccountId m_id;
    QVariantMap m_authData;
    Accounts::Manager *m_manager;
    SignOn::SessionData m_sessionData;
};

#endif //GET_CREDENTIALS_JOB_H
