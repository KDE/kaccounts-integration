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

#include "kioservices.h"
#include "createnetattachjob.h"
#include "createkioservice.h"
#include "removekioservice.h"

#include <QFile>
#include <QDirIterator>
#include <QStandardPaths>
#include <QDebug>

KIOServices::KIOServices(QObject *parent)
    : KAccountsDPlugin(parent)
{
}

void KIOServices::onAccountCreated(const Accounts::AccountId accId, const Accounts::ServiceList &serviceList)
{
    qDebug();
    Q_FOREACH(const Accounts::Service &service, serviceList) {
        if (service.serviceType() != QLatin1String("dav-storage")) {
            qDebug() << "Ignoring: " << service.serviceType();
            continue;
        }
        if (isEnabled(accId, service.name())) {
            qDebug() << "Already configured: " << service.name();
            continue;
        }

        qDebug() << "Creating: " << service.name() << "Of type: " << service.serviceType();
        enableService(accId, service);
    }
}

void KIOServices::onAccountRemoved(const Accounts::AccountId accId)
{
    qDebug();
    QString accountId = QString::number(accId) + "_";

    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    path.append(QStringLiteral("/remoteview/"));

    QDirIterator i(path, QDir::NoDotAndDotDot | QDir::Files);
    while (i.hasNext()) {
        i.next();
        if (!i.fileName().startsWith(accountId)) {
            continue;
        }

        QString serviceName = i.fileName();
        qDebug() << "Removing: " << serviceName;
        serviceName = serviceName.mid(accountId.count(), serviceName.indexOf(QLatin1String(".desktop")) - accountId.count());
        qDebug() << "Removing N: " << serviceName;
        disableService(accId, serviceName);
    }
}

void KIOServices::onServiceEnabled(const Accounts::AccountId accId, const Accounts::Service &service)
{
    if (service.serviceType() != QLatin1String("dav-storage")) {
        qDebug() << "Ignoring: " << service.serviceType();
        return;
    }
    if (isEnabled(accId, service.name())) {
        qDebug() << "Already configured: " << service.name();
        return;
    }

    enableService(accId, service);
}

void KIOServices::onServiceDisabled(const Accounts::AccountId accId, const Accounts::Service &service)
{
    if (service.serviceType() != QLatin1String("dav-storage")) {
        qDebug() << "Ignoring: " << service.serviceType();
        return;
    }
    if (!isEnabled(accId, service.name())) {
        qDebug() << "Already not configured: " << service.name();
        return;
    }

    disableService(accId, service.name());
}

void KIOServices::enableService(const Accounts::AccountId accId, const Accounts::Service &service)
{
    CreateKioService *job = new CreateKioService(this);
    job->setAccountId(accId);
    job->setServiceName(service.name());
    job->setServiceType(service.serviceType());
    job->start();
}

void KIOServices::disableService(const Accounts::AccountId accId, const QString &serviceName)
{
    RemoveKioService *job = new RemoveKioService(this);
    job->setServiceName(serviceName);
    job->setAccountId(accId);
    job->start();
}

bool KIOServices::isEnabled(const Accounts::AccountId accId, const QString &serviceName)
{
    QString uniqueId(QString::number(accId) + "_" + serviceName);

    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    path += QStringLiteral("/remoteview/") + uniqueId + QStringLiteral(".desktop");

    return QFile::exists(path);
}
