/*************************************************************************************
 *  Copyright (C) 2013 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
 *                                                                                   *
 *  This library is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU Lesser General Public                       *
 *  License as published by the Free Software Foundation; either                     *
 *  version 2 of the License, or (at your option) any later version.                 *
 *                                                                                   *
 *  This library is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 *  Library General Public License for more details.                                 *
 *                                                                                   *
 *  You should have received a copy of the GNU Library General Public License        *
 *  along with this library; see the file COPYING.LIB.  If not, write to             *
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,             *
 *  Boston, MA 02110-1301, USA.                                                      *
 *************************************************************************************/

#include "getcredentialsjob.h"

#include <Accounts/Manager>
#include <Accounts/Account>
#include <Accounts/AccountService>

#include <SignOn/Identity>

#include <KDebug>

GetCredentialsJob::GetCredentialsJob(const Accounts::AccountId& id, QObject* parent)
: KJob(parent)
, m_id(id)
, m_manager(new Accounts::Manager(this))
{

}

void GetCredentialsJob::start()
{
    QMetaObject::invokeMethod(this, "getCredentials", Qt::QueuedConnection);
}

void GetCredentialsJob::setServiceType(const QString& serviceType)
{
    m_serviceType = serviceType;
}

void GetCredentialsJob::getCredentials()
{
    Accounts::Account *acc = m_manager->account(m_id);
    if (!acc) {
        setError(-1);
        setErrorText("Could not find account");
        emitResult();
        return;
    }
    Accounts::AccountService *service = new Accounts::AccountService(acc, m_manager->service(m_serviceType), this);

    Accounts::AuthData authData = service->authData();
    m_authData = authData.parameters();
    SignOn::Identity* identity = SignOn::Identity::existingIdentity(authData.credentialsId(), this);

    if (!identity) {
        setError(-1);
        setErrorText("Could not find credentials");
        emitResult();
        return;
    }

    m_authData["AccountUsername"] = acc->value("username").toString();
    QPointer<SignOn::AuthSession> authSession = identity->createSession(authData.method());

    connect(authSession, SIGNAL(response(SignOn::SessionData)),
            SLOT(sessionResponse(SignOn::SessionData)));
    connect(authSession, SIGNAL(error(SignOn::Error)),
            SLOT(sessionError(SignOn::Error)));

    kDebug() << authData.parameters();
    kDebug() << authData.mechanism();
    authSession->process(authData.parameters(), authData.mechanism());
}

void GetCredentialsJob::sessionResponse(const SignOn::SessionData& data)
{
    kDebug() << data.toMap();
    m_sessionData = data;
    emitResult();
}

void GetCredentialsJob::sessionError(const SignOn::Error& error)
{
    kDebug() << error.message();
    setError(-1);
    setErrorText(error.message());
    emitResult();
}

Accounts::AccountId GetCredentialsJob::accountId() const
{
    return m_id;
}

QVariantMap GetCredentialsJob::credentialsData() const
{
    return m_sessionData.toMap().unite(m_authData);
}
