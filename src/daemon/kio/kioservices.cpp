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

#include <KDebug>
#include <KGlobal>
#include <KStandardDirs>

KIOServices::KIOServices(QObject* parent) : QObject(parent)
{
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");
}

void KIOServices::serviceAdded(const Accounts::AccountId& accId, QMap< QString, QString >& services)
{
    kDebug();
    QString serviceName = services.keys().first();
    QString serviceType = services.value(serviceName);
    if (serviceType != QLatin1String("dav-storage")) {
        kWarning() << "We only support dav-storage atm: " << serviceType;
        return;
    }

    QString path = KGlobal::dirs()->saveLocation("remote_entries");
    QString uniqueId(QString::number(accId) + "_" + serviceType);
    path +=  uniqueId + ".desktop";

    if (QFile::exists(path)) {
        kDebug() << "Service already exists";
        return;
    }

    CreateKioService *job = new CreateKioService(this);
    job->setAccountId(accId);
    job->setServiceName(serviceName);
    job->setServiceType(serviceType);
    job->start();
}

void KIOServices::serviceRemoved(const Accounts::AccountId& accId, QMap< QString, QString >& services)
{
    QString serviceName = services.keys().first();
    QString serviceType = services.value(serviceName);


    RemoveKioService *job = new RemoveKioService(this);
    job->setServiceName(serviceName);
    job->setAccountId(accId);
    job->start();
}

void KIOServices::serviceEnabled(const Accounts::AccountId& accId, QMap< QString, QString >& services)
{
    serviceAdded(accId, services);
}

void KIOServices::serviceDisabled(const Accounts::AccountId& accId, QMap< QString, QString >& services)
{
    serviceRemoved(accId, services);
}
