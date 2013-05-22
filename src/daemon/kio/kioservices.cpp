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

#include <QtCore/QFile>
#include <QtCore/QDirIterator>

#include <KDebug>
#include <KGlobal>
#include <KStandardDirs>

KIOServices::KIOServices(QObject* parent) : QObject(parent)
{
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");
}

void KIOServices::accountCreated(const Accounts::AccountId& accId, const Accounts::ServiceList &serviceList)
{
    kDebug();
    Q_FOREACH(const Accounts::Service &service, serviceList) {
        if (service.serviceType() != QLatin1String("dav-storage")) {
            kDebug() << "Ignoring: " << service.serviceType();
            continue;
        }
        if (isEnabled(accId, service.name())) {
            kDebug() << "Already configured: " << service.name();
            continue;
        }

        kDebug() << "Creating: " << service.name() << "Of type: " << service.serviceType();
        enableService(accId, service);
    }
}

void KIOServices::accountRemoved(const Accounts::AccountId& accId)
{
    kDebug();
    QString accountId = QString::number(accId) + "_";
    QString path = KGlobal::dirs()->saveLocation("remote_entries");

    QDirIterator i(path, QDir::NoDotAndDotDot | QDir::Files);
    while (i.hasNext()) {
        i.next();
        if (!i.fileName().startsWith(accountId)) {
            continue;
        }

        QString serviceName = i.fileName();
        kDebug() << "Removing: " << serviceName;
        serviceName = serviceName.mid(accountId.count(), serviceName.indexOf(QLatin1String(".desktop")) - accountId.count());
        kDebug() << "Removing N: " << serviceName;
        disableService(accId, serviceName);
    }
}

void KIOServices::serviceEnabled(const Accounts::AccountId& accId, const Accounts::Service &service)
{
    enableService(accId, service);
}

void KIOServices::serviceDisabled(const Accounts::AccountId& accId, const Accounts::Service &service)
{
    disableService(accId, service.name());
}

void KIOServices::enableService(const Accounts::AccountId& accId, const Accounts::Service &service)
{
    CreateKioService *job = new CreateKioService(this);
    job->setAccountId(accId);
    job->setServiceName(service.name());
    job->setServiceType(service.serviceType());
    job->start();
}

void KIOServices::disableService(const Accounts::AccountId& accId, const QString& serviceName)
{
    RemoveKioService *job = new RemoveKioService(this);
    job->setServiceName(serviceName);
    job->setAccountId(accId);
    job->start();
}

bool KIOServices::isEnabled(const Accounts::AccountId& accId, const QString &serviceName)
{
    QString path = KGlobal::dirs()->saveLocation("remote_entries");
    QString uniqueId(QString::number(accId) + "_" + serviceName);
    path +=  uniqueId + ".desktop";

    return QFile::exists(path);
}
