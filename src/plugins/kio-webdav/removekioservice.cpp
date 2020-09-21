/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "removekioservice.h"
#include "removenetattachjob.h"

#include <QFile>
#include <QDebug>

RemoveKioService::RemoveKioService(QObject *parent)
    : KJob(parent)
{

}

void RemoveKioService::start()
{
    QMetaObject::invokeMethod(this, "removeKioService", Qt::QueuedConnection);
}

void RemoveKioService::removeKioService()
{
    qDebug();
    RemoveNetAttachJob *job = new RemoveNetAttachJob(this);
    job->setUniqueId(QString::number(m_accountId) + QStringLiteral("_") + m_serviceName);
    connect(job, &RemoveNetAttachJob::finished, this, &RemoveKioService::removeNetAatachFinished);
    job->start();
}

void RemoveKioService::removeNetAatachFinished(KJob *job)
{
    qDebug();
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
    }

    emitResult();
}

Accounts::AccountId RemoveKioService::accountId() const
{
    return m_accountId;
}

void RemoveKioService::setAccountId(Accounts::AccountId accId)
{
    m_accountId = accId;
}

QString RemoveKioService::serviceName() const
{
    return m_serviceName;
}

void RemoveKioService::setServiceName(const QString &serviceName)
{
    m_serviceName = serviceName;
}
