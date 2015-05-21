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

#include "removetask.h"
#include "jobs/removeakonadiresource.h"

#include "google_calendar_settings.h"

#include <akonadi/agentmanager.h>

#include <QDebug>

using namespace Akonadi;

RemoveTask::RemoveTask(KConfigGroup group, QObject* parent)
 : KJob(parent)
 , m_config(group)
{
    setObjectName(m_config.name());
    setProperty("type", QVariant::fromValue(m_config.readEntry("type")));
}

RemoveTask::~RemoveTask()
{

}

void RemoveTask::start()
{
    QMetaObject::invokeMethod(this, "deleteTask", Qt::QueuedConnection);
}

void RemoveTask::deleteTask()
{
    KConfigGroup services = m_config.group("services");
    if (services.readEntry("Calendar", 0) == 0) {
        removeResource();
        return;
    }

    removeTaskInResource();
}

void RemoveTask::removeResource()
{
    RemoveAkonadiResource *remove = new RemoveAkonadiResource("calendarAndTasksResource", "Tasks", m_config, this);
    connect(remove, SIGNAL(finished(KJob*)), this, SLOT(resourceRemoved()));
    remove->start();
}

void RemoveTask::removeTaskInResource()
{
    org::kde::Akonadi::GoogleCalendar::Settings *calendarSettings = new org::kde::Akonadi::GoogleCalendar::Settings(m_config.group("private").readEntry("calendarAndTasksResource"), "/Settings", QDBusConnection::sessionBus());
    calendarSettings->setTaskLists(QStringList());
    calendarSettings->deleteLater();

    QString id = m_config.group("private").readEntry("calendarAndTasksResource").remove("org.freedesktop.Akonadi.Resource.");
    AgentInstance agent = AgentManager::self()->instance(id);
    if (agent.isValid()) {
        agent.reconfigure();
    } else {
        qDebug() << "Agent not found, removing tasks anyway";
    }

    //Since removeCalendar and removeTask jobs can be run in paralel we have to check in the last step
    //if the other has been removed to remove the resource
    if (m_config.group("services").readEntry("Calendar", 0) == 0) {
        removeResource();
        return;
    }

    m_config.group("services").writeEntry("Tasks", 0);
    m_config.sync();
    emitResult();
}

void RemoveTask::resourceRemoved()
{
    m_config.group("services").writeEntry("Tasks", 0);
    m_config.sync();
    emitResult();
}