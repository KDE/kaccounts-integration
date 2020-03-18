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

#include "createkioservice.h"
#include "createnetattachjob.h"
#include "../lib/getcredentialsjob.h"
#include <core.h>

#include <QDebug>
#include <Accounts/Manager>

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

    GetCredentialsJob *gjob = qobject_cast<GetCredentialsJob*>(job);
    QVariantMap data = gjob->credentialsData();

    Accounts::Service service = m_manager->service(m_serviceName);
    QString host = m_account->value("dav/host").toString();
    QString path = m_account->value("dav/storagePath").toString();

    m_account->selectService(service);
    QString username = data["UserName"].toString();
    CreateNetAttachJob *netJob = new CreateNetAttachJob(this);
    connect(netJob, &CreateNetAttachJob::finished, this, &CreateKioService::netAttachCreated);

    netJob->setHost(host);
    netJob->setPath(path);
    netJob->setUsername(username);
    netJob->setPassword(data["Secret"].toString());
    netJob->setIcon(service.iconName());
    netJob->setUniqueId(QString::number(m_accountId) + "_" + m_serviceName);
    netJob->setName(m_manager->provider(service.provider()).displayName() + " " + service.displayName());
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
