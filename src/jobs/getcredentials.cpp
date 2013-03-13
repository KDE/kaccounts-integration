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

#include "getcredentials.h"

#include <QtCore/QDebug>

#include <Accounts/Manager>
#include <Accounts/Account>
#include <Accounts/AccountService>

#include <SignOn/Identity>

GetCredentials::GetCredentials(const Accounts::AccountId& id, QObject* parent)
 : KJob(parent)
 , m_id(id)
 , m_manager(new Accounts::Manager())
{

}

void GetCredentials::start()
{
    QMetaObject::invokeMethod(this, "getCredentials", Qt::QueuedConnection);
}

void GetCredentials::setServiceType(const QString& serviceType)
{
    m_serviceType = serviceType;
}

void GetCredentials::getCredentials()
{
    Accounts::Account* acc = m_manager->account(m_id);
    Accounts::AccountService *service = new Accounts::AccountService(acc, m_manager->service(m_serviceType), this);

    Accounts::AuthData authData = service->authData();
    SignOn::Identity* identity = SignOn::Identity::existingIdentity(authData.credentialsId(), this);

    QPointer<SignOn::AuthSession> authSession = identity->createSession(authData.method());

    connect(authSession, SIGNAL(response(SignOn::SessionData)),
                    SLOT(sessionResponse(SignOn::SessionData)));
    connect(authSession, SIGNAL(error(SignOn::Error)),
                    SLOT(sessionError(SignOn::Error)));

    qDebug() << authData.parameters();
    qDebug() << authData.mechanism();
    authSession->process(authData.parameters(), authData.mechanism());
}

void GetCredentials::info(const SignOn::IdentityInfo& info)
{
}

void GetCredentials::sessionResponse(const SignOn::SessionData& data)
{
    qDebug() << data.toMap();
    Q_EMIT gotCredentials(data);
}

void GetCredentials::sessionError(const SignOn::Error& error)
{
    qDebug() << error.message();
}