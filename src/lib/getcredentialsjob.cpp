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
    QString serviceType;
    QString authMechanism;
    QString authMethod;
    Accounts::AccountId id;
    QVariantMap authData;
    Accounts::Manager *manager;
    SignOn::SessionData sessionData;
    uint repeatedTries;
};

GetCredentialsJob::GetCredentialsJob(const Accounts::AccountId &id, QObject *parent)
    : KJob(parent)
    , d(new Private)
{
    d->id = id;
    d->manager = KAccounts::accountsManager();
    d->repeatedTries = 0;
}


GetCredentialsJob::GetCredentialsJob(const Accounts::AccountId &id, const QString &authMethod, const QString &authMechanism, QObject *parent)
    : KJob(parent)
    , d(new Private)
{
    d->id = id;
    d->manager = KAccounts::accountsManager();
    d->authMechanism = authMechanism;
    d->authMethod = authMethod;
}

void GetCredentialsJob::start()
{
    QMetaObject::invokeMethod(this, "getCredentials", Qt::QueuedConnection);
}

void GetCredentialsJob::setServiceType(const QString& serviceType)
{
    d->serviceType = serviceType;
}

void GetCredentialsJob::getCredentials()
{
    Accounts::Account *acc = d->manager->account(d->id);
    if (!acc) {
        qWarning() << "Unable to find account for id" << d->id;
        if (d->repeatedTries < 3) {
            qDebug() << "Retrying in 2s";
            QTimer *repeatTimer = new QTimer(this);
            repeatTimer->setSingleShot(true);
            connect(repeatTimer, SIGNAL(timeout()), this, SLOT(getCredentials()));
            repeatTimer->start(2000);
            d->repeatedTries++;
        } else {
            setError(KJob::UserDefinedError);
            setErrorText(QLatin1String("Could not find account"));
            emitResult();
        }
        return;
    }

    Accounts::AccountService *service = new Accounts::AccountService(acc, d->manager->service(d->serviceType), this);

    Accounts::AuthData authData = service->authData();
    d->authData = authData.parameters();
    SignOn::Identity* identity = SignOn::Identity::existingIdentity(authData.credentialsId(), this);

    if (!identity) {
        qWarning() << "Unable to find identity for account id" << d->id;
        setError(KJob::UserDefinedError);
        setErrorText(QLatin1String("Could not find credentials"));
        emitResult();
        return;
    }

    d->authData["AccountUsername"] = acc->value(QLatin1String("username")).toString();
    QPointer<SignOn::AuthSession> authSession = identity->createSession(d->authMethod.isEmpty() ? authData.method() : d->authMethod);

    connect(authSession.data(), &SignOn::AuthSession::response,
            [this, identity](const SignOn::SessionData &data) {
                d->sessionData = data;
                emitResult();
            });

    connect(authSession.data(), &SignOn::AuthSession::error,
            [this](const SignOn::Error &error) {
                qDebug() << error.message();
                setError(KJob::UserDefinedError);
                setErrorText(error.message());
                emitResult();
            });

    authSession->process(authData.parameters(), d->authMechanism.isEmpty() ? authData.mechanism() : d->authMechanism);
}

Accounts::AccountId GetCredentialsJob::accountId() const
{
    return d->id;
}

QVariantMap GetCredentialsJob::credentialsData() const
{
    return d->sessionData.toMap().unite(d->authData);
}
