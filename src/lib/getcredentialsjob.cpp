/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "getcredentialsjob.h"
#include "core.h"

#include <Accounts/Account>
#include <Accounts/AccountService>
#include <Accounts/Manager>

#include <SignOn/Identity>

#include <KLocalizedString>

#include <QDebug>
#include <QTimer>

class GetCredentialsJob::Private
{
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
            q->setErrorText(i18n("Could not find account"));
            q->emitResult();
        }
        return;
    }

    Accounts::AccountService *service = new Accounts::AccountService(acc, manager->service(serviceType), q);

    Accounts::AuthData serviceAuthData = service->authData();
    authData = serviceAuthData.parameters();
    SignOn::Identity *identity = SignOn::Identity::existingIdentity(acc->credentialsId(), q);

    if (!identity) {
        qWarning() << "Unable to find identity for account id" << id;
        q->setError(KJob::UserDefinedError);
        q->setErrorText(i18n("Could not find credentials"));
        q->emitResult();
        return;
    }

    authData[QStringLiteral("AccountUsername")] = acc->value(QStringLiteral("username")).toString();
    QPointer<SignOn::AuthSession> authSession = identity->createSession(authMethod.isEmpty() ? serviceAuthData.method() : authMethod);
    if (!authSession) {
        qWarning() << "Unable to create auth session for" << authMethod << serviceAuthData.method();
        q->setError(KJob::UserDefinedError);
        q->setErrorText(i18n("Could not create auth session"));
        q->emitResult();
        return;
    }

    QObject::connect(authSession.data(), &SignOn::AuthSession::response, q, [this](const SignOn::SessionData &data) {
        sessionData = data;
        q->emitResult();
    });

    QObject::connect(authSession.data(), &SignOn::AuthSession::error, q, [this](const SignOn::Error &error) {
        qDebug() << error.message();
        q->setError(KJob::UserDefinedError);
        q->setErrorText(error.message());
        q->emitResult();
    });

    authSession->process(serviceAuthData.parameters(), authMechanism.isEmpty() ? serviceAuthData.mechanism() : authMechanism);
}

GetCredentialsJob::GetCredentialsJob(Accounts::AccountId id, QObject *parent)
    : KJob(parent)
    , d(new Private(this))
{
    d->id = id;
    d->manager = KAccounts::accountsManager();
    d->repeatedTries = 0;
    d->serviceType = QString();
}

GetCredentialsJob::GetCredentialsJob(Accounts::AccountId id, const QString &authMethod, const QString &authMechanism, QObject *parent)
    : KJob(parent)
    , d(new Private(this))
{
    d->id = id;
    d->manager = KAccounts::accountsManager();
    d->authMechanism = authMechanism;
    d->authMethod = authMethod;
    d->repeatedTries = 0;
    d->serviceType = QString();
}

GetCredentialsJob::~GetCredentialsJob()
{
    delete d;
}

void GetCredentialsJob::start()
{
    QMetaObject::invokeMethod(this, "getCredentials", Qt::QueuedConnection);
}

void GetCredentialsJob::setServiceType(const QString &serviceType)
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
