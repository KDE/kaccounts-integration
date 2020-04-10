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
