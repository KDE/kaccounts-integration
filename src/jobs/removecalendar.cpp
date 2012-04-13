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

#include "removecalendar.h"
#include "removeakonadiresource.h"
#include "google_calendar_settings.h"

RemoveCalendar::RemoveCalendar(KConfigGroup group, QObject* parent)
 : KJob(parent)
 , m_config(group)
{
    setObjectName(m_config.name());
}

RemoveCalendar::~RemoveCalendar()
{

}

void RemoveCalendar::start()
{
    QMetaObject::invokeMethod(this, "deleteCalendar", Qt::QueuedConnection);
}

void RemoveCalendar::deleteCalendar()
{
    KConfigGroup services = m_config.group("services");
    if (services.readEntry("Tasks", 0) == 0) {
        removeResource();
        return;
    }

    removeCalendarsInResource();
}

void RemoveCalendar::removeResource()
{
    RemoveAkonadiResource *remove = new RemoveAkonadiResource("calendarAndTasksResource", m_config, this);
    connect(remove, SIGNAL(finished(KJob*)), this, SLOT(resourceRemoved()));
    remove->start();
}

void RemoveCalendar::removeCalendarsInResource()
{
    org::kde::Akonadi::GoogleCalendar::Settings *calendarSettings = new org::kde::Akonadi::GoogleCalendar::Settings(m_config.group("private").readEntry("calendarAndTasksResource"), "/Settings", QDBusConnection::sessionBus());
    calendarSettings->setCalendars(QStringList());
    calendarSettings->writeConfig();

    calendarSettings->deleteLater();

    if (m_config.group("services").readEntry("Tasks", 0) == 0) {
        removeResource();
        return;
    }

    m_config.group("services").writeEntry("Calendar", 0);
    emitResult();
}

void RemoveCalendar::resourceRemoved()
{
    m_config.group("services").writeEntry("Calendar", 0);
    emitResult();
}

