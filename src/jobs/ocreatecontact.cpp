/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#include "ocreatecontact.h"
#include "davGroupware_settings.h"

#include <QtGui/QWidget>
#include <QtGui/QApplication>
#include <QtCore/QDebug>

#include <QtDBus/QDBusInterface>

#include <libkgoogle/auth.h>
#include <libkgoogle/reply.h>
#include <libkgoogle/request.h>
#include <libkgoogle/accessmanager.h>
#include <libkgoogle/objects/calendar.h>
#include <libkgoogle/services/calendar.h>

#include <akonadi/agenttype.h>
#include <akonadi/agentmanager.h>
#include <akonadi/agentinstancecreatejob.h>
#include <unistd.h>

#include <KWallet/Wallet>

using namespace KWallet;
using namespace Akonadi;
using namespace KGoogle;

OCreateContact::OCreateContact(KConfigGroup group, QObject* parent)
: KJob(parent)
, m_config(group)
{
}

OCreateContact::~OCreateContact()
{

}

void OCreateContact::start()
{
    m_config.sync();
    if (m_config.hasGroup("private")  && m_config.group("private").hasKey("contactAndCalendarResource")) {
        QMetaObject::invokeMethod(this, "useCalendarResource", Qt::QueuedConnection);
        return;
    }
    QMetaObject::invokeMethod(this, "createResource", Qt::QueuedConnection);
}

KConfigGroup OCreateContact::config() const
{
    return m_config;
}

void OCreateContact::createResource()
{
    qDebug() << "Creating new resource";
    const AgentType type = AgentManager::self()->type("akonadi_davgroupware_resource");

    AgentInstanceCreateJob *job = new AgentInstanceCreateJob( type, this );
    connect( job, SIGNAL(result(KJob*)), SLOT(resourceCreated(KJob*)) );
    job->start();
}

const QString OCreateContact::davUrl()
{
    KUrl url(m_config.readEntry("server", ""));
    url.addPath("apps/contacts/carddav.php/");

    QString str("$default$|CardDav|");
    str.append(url.url());

    m_config.group("services").writeEntry("Contact", 1);

    return str;
}

void OCreateContact::resourceCreated(KJob* job)
{
    if (job->error()) {
        setError(-1);
        emitResult();
        return;
    }

    AgentInstance agent = qobject_cast<AgentInstanceCreateJob*>( job )->instance();
    agent.setName(m_config.name() + " " + i18n("Calendar and Contacts"));

    QString service = "org.freedesktop.Akonadi.Resource." + agent.identifier();

    KConfigGroup privates(&m_config, "private");
    privates.writeEntry("contactAndCalendarResource", service);
    m_config.sync();

    org::kde::Akonadi::davGroupware::Settings *settings = new org::kde::Akonadi::davGroupware::Settings(service, "/Settings", QDBusConnection::sessionBus());
    settings->setDefaultUsername(m_config.name());
    settings->setRemoteUrls(QStringList(davUrl()));
    settings->setDisplayName(m_config.name());
    settings->setSettingsVersion(2);

    settings->writeConfig();
    agent.reconfigure();

    WId windowId = 0;
    if (qApp->activeWindow()) {
        windowId = qApp->activeWindow()->winId();
    }
    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), windowId, Wallet::Synchronous);

    if (!wallet->isOpen() || !wallet->isEnabled()) {
        setError(-1);
        emitResult();
        return;
    }

    QString password;
    wallet->setFolder("WebAccounts");
    wallet->readPassword("owncloud-" + m_config.name(), password);

    QString key (agent.identifier());
    key.append(",$default$");

    wallet->setFolder(Wallet::PasswordFolder());
    wallet->writePassword(key, password);

    agent.synchronize();

    emitResult();
}

void OCreateContact::useCalendarResource()
{
    QString resource = m_config.group("private").readEntry("contactAndCalendarResource");
    org::kde::Akonadi::davGroupware::Settings *settings = new org::kde::Akonadi::davGroupware::Settings(resource, "/Settings", QDBusConnection::sessionBus());

    QStringList list = settings->remoteUrls().value();
    list.append(davUrl());
    qDebug() << list;
    settings->setRemoteUrls(list).waitForFinished();
    settings->setDefaultUsername(m_config.name()).waitForFinished();

    settings->writeConfig().waitForFinished();

    resource.remove("org.freedesktop.Akonadi.Resource.");

    AgentInstance instance = AgentManager::self()->instance(resource);
    if (!instance.isValid()) {
        return;
    }

    instance.reconfigure();
    instance.synchronize();

    emitResult();
}