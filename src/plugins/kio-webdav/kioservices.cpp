/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kioservices.h"
#include "createkioservice.h"
#include "createnetattachjob.h"
#include "removekioservice.h"

#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QStandardPaths>

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(KIOServices, "kio-webdav.json")

KIOServices::KIOServices(QObject *parent, const QVariantList &args)
    : KAccountsDPlugin(parent, args)
{
}

KIOServices::~KIOServices() = default;

void KIOServices::onAccountCreated(const Accounts::AccountId accId, const Accounts::ServiceList &serviceList)
{
    qDebug();
    for (const Accounts::Service &service : serviceList) {
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
    const QString accountId = QString::number(accId) + QStringLiteral("_");

    const QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/remoteview/");

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
    const QString uniqueId(QString::number(accId) + QStringLiteral("_") + serviceName);

    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    path += QStringLiteral("/remoteview/") + uniqueId + QStringLiteral(".desktop");

    return QFile::exists(path);
}

#include "kioservices.moc"
