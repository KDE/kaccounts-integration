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

#include "enableservicejob.h"
#include "fetchsettingsjob.h"
#include "changesettingsjob.h"

#include <KDebug>

EnableServiceJob::EnableServiceJob(QObject* parent): KJob(parent)
{

}

void EnableServiceJob::start()
{
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void EnableServiceJob::init()
{
    FetchSettingsJob *job = new FetchSettingsJob(this);
    connect(job, SIGNAL(finished(KJob*)), SLOT(fetchSettingsJobDone(KJob*)));

    if (!m_interface.isEmpty()) {
        job->setInterface(m_interface);
    }
    job->setResourceId(m_resourceId);
    job->setKey("accountServices");
    job->start();
}

void EnableServiceJob::fetchSettingsJobDone(KJob* job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    FetchSettingsJob *fetchJob = qobject_cast<FetchSettingsJob*>(job);
    QStringList services = fetchJob->value<QStringList>();
    if (services.contains(m_service) && m_serviceStatus == Enable) {
        kDebug() << "Already enabled service: " << m_service;
        emitResult();
        return;
    }
    if (!services.contains(m_service) && m_serviceStatus == Disable) {
        kDebug() << "Trying to disable a not enabled service: " << m_service;
        emitResult();
        return;
    }

    if (m_serviceStatus == Enable) {
        services.append(m_service);
    } else {
        services.removeAll(m_service);
    }

    ChangeSettingsJob *changeJob = new ChangeSettingsJob(this);
    connect(changeJob, SIGNAL(finished(KJob*)), SLOT(changeSettingsDone(KJob*)));
    if (!m_interface.isEmpty()) {
        changeJob->setInterface(m_interface);
    }
    changeJob->setResourceId(m_resourceId);

    changeJob->setSetting("setAccountServices", services);
    changeJob->start();
}

void EnableServiceJob::changeSettingsDone(KJob* job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
    }

    emitResult();
}

Accounts::AccountId EnableServiceJob::accountId() const
{
    return m_accountId;
}

void EnableServiceJob::setAccountId(const Accounts::AccountId& accId)
{
    m_accountId = accId;
}

QString EnableServiceJob::interface() const
{
    return m_interface;
}

void EnableServiceJob::setInterface(const QString& interface)
{
    m_interface = interface;
}

QString EnableServiceJob::resourceId() const
{
    return m_resourceId;
}

void EnableServiceJob::setResourceId(const QString& resourceId)
{
    m_resourceId = resourceId;
}

QString EnableServiceJob::service()
{
    return m_service;
}

void EnableServiceJob::setService(const QString& serviceName, EnableServiceJob::Status status)
{
    m_service = serviceName;
    m_serviceStatus = status;
}