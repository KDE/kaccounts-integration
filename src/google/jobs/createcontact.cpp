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

#include "createcontact.h"
#include "google_contact_settings.h"

#include <QDBusInterface>

#include <libkgoogle/auth.h>
#include <libkgoogle/reply.h>
#include <libkgoogle/request.h>
#include <libkgoogle/accessmanager.h>
#include <libkgoogle/objects/contact.h>
#include <libkgoogle/services/contacts.h>

#include <akonadi/agenttype.h>
#include <akonadi/agentmanager.h>
#include <akonadi/agentinstancecreatejob.h>

using namespace KGoogle;
using namespace Akonadi;

CreateContact::CreateContact(KConfigGroup group, QObject* parent)
: KJob(parent)
, m_config(group)
{
    Auth *auth = Auth::instance();
    auth->init("Akonadi Google", "554041944266.apps.googleusercontent.com", "mdT1DjzohxN3npUUzkENT0gO");
    qRegisterMetaType< KGoogle::Services::Contacts >("Contact");
}

CreateContact::~CreateContact()
{

}

void CreateContact::start()
{
    QMetaObject::invokeMethod(this, "createResource", Qt::QueuedConnection);
}

void CreateContact::createResource()
{
    const AgentType type = AgentManager::self()->type("akonadi_googlecontacts_resource");

    AgentInstanceCreateJob *job = new AgentInstanceCreateJob( type, this );
    connect( job, SIGNAL(result(KJob*)), SLOT(resourceCreated(KJob*)) );
    job->start();
}

void CreateContact::resourceCreated(KJob* job)
{
    if (job->error()) {
        setError(-1);
        m_config.group("services").writeEntry("Contact", -1);
        emitResult();
        return;
    }

    AgentInstance agent = qobject_cast<AgentInstanceCreateJob*>( job )->instance();
    agent.setName(m_config.name() + " Contact");

    QString service = "org.freedesktop.Akonadi.Resource." + agent.identifier();
    KConfigGroup privates(&m_config, "private");
    privates.writeEntry("contactResource", service);
    privates.sync();

    org::kde::Akonadi::GoogleContacts::Settings *settings = new org::kde::Akonadi::GoogleContacts::Settings(service, "/Settings", QDBusConnection::sessionBus());
    settings->setAccount(m_config.name());

    agent.reconfigure();

    m_config.group("services").writeEntry("Contact", 1);
    emitResult();
}
