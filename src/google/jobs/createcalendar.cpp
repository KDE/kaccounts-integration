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

#include "createcalendar.h"
#include "google_calendar_settings.h"

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

#include <KDebug>

using namespace Akonadi;
using namespace KGoogle;

CreateCalendar::CreateCalendar(KConfigGroup group, QObject* parent)
: KJob(parent)
, m_config(group)
{
    Auth *auth = Auth::instance();
    auth->init("Akonadi Google", "554041944266.apps.googleusercontent.com", "mdT1DjzohxN3npUUzkENT0gO");
    qRegisterMetaType< KGoogle::Services::Calendar >("Calendar");
}

CreateCalendar::~CreateCalendar()
{
//     m_accessManager->deleteLater();
}

void CreateCalendar::start()
{
    m_config.sync();
    if (m_config.hasGroup("private")  && m_config.group("private").hasKey("calendarAndTasksResource")) {
        QMetaObject::invokeMethod(this, "useTaskResource", Qt::QueuedConnection);
        return;
    }
    QMetaObject::invokeMethod(this, "createResource", Qt::QueuedConnection);
}

KConfigGroup CreateCalendar::config() const
{
    return m_config;
}

void CreateCalendar::createResource()
{
    kDebug() << "Creating new resource";
    const AgentType type = AgentManager::self()->type("akonadi_googlecalendar_resource");

    AgentInstanceCreateJob *job = new AgentInstanceCreateJob( type, this );
    connect( job, SIGNAL(result(KJob*)), SLOT(resourceCreated(KJob*)) );
    job->start();
}

void CreateCalendar::resourceCreated(KJob* job)
{
    if (job->error()) {
        setError(-1);
        m_config.group("services").writeEntry("Calendar", -1);
        m_config.sync();
        emitResult();
        return;
    }

    m_agent = qobject_cast<AgentInstanceCreateJob*>( job )->instance();
    m_agent.setName(m_config.name() + " " + i18n("Calendar / Tasks"));

    QString service = "org.freedesktop.Akonadi.Resource." + m_agent.identifier();

    KConfigGroup privates(&m_config, "private");
    privates.writeEntry("calendarAndTasksResource", service);
    m_config.sync();

    m_calendarSettings = new org::kde::Akonadi::GoogleCalendar::Settings(service, "/Settings", QDBusConnection::sessionBus());
    m_calendarSettings->setAccount(m_config.name());
    m_calendarSettings->writeConfig();

    fetchDefaultCollections();
}

void CreateCalendar::fetchDefaultCollections()
{
    m_accessManager = new AccessManager;

    connect(m_accessManager, SIGNAL(replyReceived(KGoogle::Reply*)),
            this, SLOT(replyReceived(KGoogle::Reply*)));

    Request *request = new Request(Services::Calendar::fetchCalendarsUrl(), Request::FetchAll, "Calendar",  Auth::instance()->getAccount(m_config.name()));
    m_accessManager->sendRequest(request);
}

void CreateCalendar::useTaskResource()
{
    m_calendarSettings = new org::kde::Akonadi::GoogleCalendar::Settings(m_config.group("private").readEntry("calendarAndTasksResource"), "/Settings", QDBusConnection::sessionBus());

    QString id = m_config.group("private").readEntry("calendarAndTasksResource").remove("org.freedesktop.Akonadi.Resource.");
    m_agent = AgentManager::self()->instance(id);

    fetchDefaultCollections();
}

void CreateCalendar::replyReceived(KGoogle::Reply* reply)
{
    QStringList calendars;
    QList< KGoogle::Object* > objects = reply->replyData();

    Q_FOREACH(KGoogle::Object * object, objects) {
        Objects::Calendar *calendar;

        calendar = static_cast< Objects::Calendar* >(object);
        calendars.append(calendar->uid());
    }

    m_calendarSettings->setCalendars(calendars);
    m_agent.reconfigure();

    delete reply;

    m_config.group("services").writeEntry("Calendar", 1);
    m_config.sync();

    emitResult();
}