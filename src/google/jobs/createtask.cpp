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

#include "createtask.h"
#include "google_calendar_settings.h"

#include <libkgoogle/auth.h>
#include <libkgoogle/reply.h>
#include <libkgoogle/request.h>
#include <libkgoogle/accessmanager.h>
#include <libkgoogle/objects/tasklist.h>
#include <libkgoogle/services/tasks.h>

#include <KDebug>

using namespace KGoogle;

CreateTask::CreateTask(KConfigGroup group, QObject* parent) : CreateCalendar(group, parent)
{
    qRegisterMetaType< KGoogle::Services::Tasks >("Task");
}

CreateTask::~CreateTask()
{
}

void CreateTask::start()
{
    CreateCalendar::start();
}

void CreateTask::fetchDefaultCollections()
{
    m_accessManager = new AccessManager;

    connect(m_accessManager, SIGNAL(replyReceived(KGoogle::Reply*)),
            this, SLOT(replyReceived(KGoogle::Reply*)));

    Request *request = new Request(Services::Tasks::fetchTaskListsUrl(), Request::FetchAll, "Task",  Auth::instance()->getAccount(m_config.name()));
    m_accessManager->sendRequest(request);
}

void CreateTask::replyReceived(KGoogle::Reply* reply)
{
    QStringList tasks;
    QList< KGoogle::Object* > objects = reply->replyData();

    Q_FOREACH(KGoogle::Object * object, objects) {
        Objects::TaskList *task;

        task = static_cast< Objects::TaskList* >(object);
        kDebug() << task->uid();
        tasks.append(task->uid());
        delete task;
    }

    m_calendarSettings->setTaskLists(tasks);
    m_agent.reconfigure();

    delete reply;

    m_config.group("services").writeEntry("Tasks", 1);
    m_config.sync();

    emitResult();
}
