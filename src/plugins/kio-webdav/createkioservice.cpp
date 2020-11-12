/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "createkioservice.h"
#include "../lib/getcredentialsjob.h"
#include "createnetattachjob.h"
#include <core.h>

#include <Accounts/Manager>
#include <QDebug>

CreateKioService::CreateKioService(QObject *parent)
    : KJob(parent)
{
    m_manager = KAccounts::accountsManager();
}

CreateKioService::~CreateKioService()
{
    delete m_account;
}

void CreateKioService::start()
{
    QMetaObject::invokeMethod(this, "createKioService", Qt::QueuedConnection);
}

void CreateKioService::createKioService()
{
    qDebug();
    m_account = m_manager->account(m_accountId);

    GetCredentialsJob *job = new GetCredentialsJob(m_accountId, QString(), QString(), this);
    connect(job, &GetCredentialsJob::finished, this, &CreateKioService::gotCredentials);
    job->setServiceType(m_serviceType);
    job->start();
}

void CreateKioService::gotCredentials(KJob *job)
{
    qDebug();
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    GetCredentialsJob *gjob = qobject_cast<GetCredentialsJob *>(job);
    const QVariantMap data = gjob->credentialsData();

    const Accounts::Service service = m_manager->service(m_serviceName);
    const QString host = m_account->value(QStringLiteral("dav/host")).toString();
    const QString path = m_account->value(QStringLiteral("dav/storagePath")).toString();

    m_account->selectService(service);
    const QString username = data[QStringLiteral("UserName")].toString();
    CreateNetAttachJob *netJob = new CreateNetAttachJob(this);
    connect(netJob, &CreateNetAttachJob::finished, this, &CreateKioService::netAttachCreated);

    netJob->setHost(host);
    netJob->setPath(path);
    netJob->setUsername(username);
    netJob->setPassword(data[QStringLiteral("Secret")].toString());
    netJob->setIcon(service.iconName());
    netJob->setUniqueId(QString::number(m_accountId) + QLatin1Char('_') + m_serviceName);
    netJob->setName(m_manager->provider(service.provider()).displayName() + QLatin1Char(' ') + service.displayName());
    netJob->start();
}

void CreateKioService::netAttachCreated(KJob *job)
{
    emitResult();
}

Accounts::AccountId CreateKioService::accountId() const
{
    return m_accountId;
}

void CreateKioService::setAccountId(const Accounts::AccountId accId)
{
    m_accountId = accId;
}

QString CreateKioService::serviceName() const
{
    return m_serviceName;
}

void CreateKioService::setServiceName(const QString &serviceName)
{
    m_serviceName = serviceName;
}

QString CreateKioService::serviceType() const
{
    return m_serviceType;
}

void CreateKioService::setServiceType(const QString &serviceType)
{
    m_serviceType = serviceType;
}
