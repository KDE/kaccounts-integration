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
#include "core.h"

#include <Accounts/Manager>
#include <Accounts/Account>
#include <Accounts/AccountService>

#include <SignOn/Identity>

#include <QDebug>
#include <QTimer>

class GetCredentialsJob::Private {
public:
    Private(GetCredentialsJob *job)
    {
        q = job;
    }

    QString serviceType;
    QString authMechanism;
    QString authMethod;
    Accounts::AccountId id;
    QVariantMap authData;
    Accounts::Manager *manager;
    SignOn::SessionData sessionData;
    uint repeatedTries;
    GetCredentialsJob *q;

    void getCredentials();
};

void GetCredentialsJob::Private::getCredentials()
{
    Accounts::Account *acc = manager->account(id);
    if (!acc) {
        qWarning() << "Unable to find account for id" << id;
        if (repeatedTries < 3) {
            qDebug() << "Retrying in 2s";
            QTimer::singleShot(2000, q, SLOT(getCredentials()));
            repeatedTries++;
        } else {
            qDebug() << repeatedTries << "ending with error";
            q->setError(KJob::UserDefinedError);
            q->setErrorText(QLatin1String("Could not find account"));
            q->emitResult();
        }
        return;
    }

    Accounts::AccountService *service = new Accounts::AccountService(acc, manager->service(serviceType), q);

    Accounts::AuthData serviceAuthData = service->authData();
    authData = serviceAuthData.parameters();
    SignOn::Identity *identity = SignOn::Identity::existingIdentity(serviceAuthData.credentialsId(), q);

    if (!identity) {
        qWarning() << "Unable to find identity for account id" << id;
        q->setError(KJob::UserDefinedError);
        q->setErrorText(QLatin1String("Could not find credentials"));
        q->emitResult();
        return;
    }

    authData["AccountUsername"] = acc->value(QLatin1String("username")).toString();
    QPointer<SignOn::AuthSession> authSession = identity->createSession(authMethod.isEmpty() ? serviceAuthData.method() : authMethod);
    if (!authSession) {
        qWarning() << "Unable to create auth session for" << authMethod << serviceAuthData.method();
        q->setError(KJob::UserDefinedError);
        q->setErrorText(QLatin1String("Could not create auth session"));
        q->emitResult();
        return;
    }

    QObject::connect(authSession.data(), &SignOn::AuthSession::response,
            [this, identity](const SignOn::SessionData &data) {
                sessionData = data;
                q->emitResult();
            });

    QObject::connect(authSession.data(), &SignOn::AuthSession::error,
            [this](const SignOn::Error &error) {
                qDebug() << error.message();
                q->setError(KJob::UserDefinedError);
                q->setErrorText(error.message());
                q->emitResult();
            });

    authSession->process(serviceAuthData.parameters(), authMechanism.isEmpty() ? serviceAuthData.mechanism() : authMechanism);
}

GetCredentialsJob::GetCredentialsJob(const Accounts::AccountId &id, QObject *parent)
    : KJob(parent)
    , d(new Private(this))
{
    d->id = id;
    d->manager = KAccounts::accountsManager();
    d->repeatedTries = 0;
}


GetCredentialsJob::GetCredentialsJob(const Accounts::AccountId &id, const QString &authMethod, const QString &authMechanism, QObject *parent)
    : KJob(parent)
    , d(new Private(this))
{
    d->id = id;
    d->manager = KAccounts::accountsManager();
    d->authMechanism = authMechanism;
    d->authMethod = authMethod;
    d->repeatedTries = 0;
}

void GetCredentialsJob::start()
{
    QMetaObject::invokeMethod(this, "getCredentials", Qt::QueuedConnection);
}

void GetCredentialsJob::setServiceType(const QString& serviceType)
{
    d->serviceType = serviceType;
}

Accounts::AccountId GetCredentialsJob::accountId() const
{
    return d->id;
}

QVariantMap GetCredentialsJob::credentialsData() const
{
    return d->sessionData.toMap().unite(d->authData);
}

#include "moc_getcredentialsjob.cpp"
