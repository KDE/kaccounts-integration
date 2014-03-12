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

#include <libkgapi2/account.h>
#include <libkgapi2/tasks/tasklist.h>
#include <libkgapi2/tasks/tasklistfetchjob.h>

#include <KDebug>

using namespace KGAPI2;

CreateTask::CreateTask(KConfigGroup group, QObject* parent) : CreateCalendar(group, parent)
{
}

CreateTask::~CreateTask()
{
    if (m_config.group("services").readEntry("Tasks", -1) == 2) {
        m_config.group("services").writeEntry("Tasks", -1);
    }
}

void CreateTask::start()
{
    kDebug();
    CreateCalendar::start();
}

void CreateTask::startByCalendar()
{
    kDebug();
    start();
}

void CreateTask::fetchDefaultCollections()
{
    AccountPtr acc(new Account);
    acc->setAccountName(m_config.name());
    acc->setAccessToken(m_accInfo[QLatin1String("accessToken")]);
    acc->setRefreshToken(m_accInfo[QLatin1String("refreshToken")]);

    const QStringList scopes = m_accInfo[QLatin1String( "scopes" )].split( QLatin1Char(','), QString::SkipEmptyParts );
    QList<QUrl> scopeUrls;
    Q_FOREACH( const QString &scope, scopes ) {
        scopeUrls << QUrl( scope );
    }
    acc->setScopes(scopeUrls);

    TaskListFetchJob *job = new TaskListFetchJob(acc, this);
    connect(job, SIGNAL(finished(KGAPI2::Job*)), SLOT(replyReceived(KGAPI2::Job*)));
}

void CreateTask::replyReceived(KGAPI2::Job* job)
{
    TaskListFetchJob *cJob = qobject_cast<TaskListFetchJob*>(job);
    if (!cJob || cJob->error()) {
        finishWithError();
        return;
    }
    QStringList tasks;
    ObjectsList objects = cJob->items();
    Q_FOREACH(ObjectPtr object, objects) {
        const TaskListPtr taskList = object.dynamicCast<TaskList>();
        tasks.append(taskList->uid());
    }

    m_calendarSettings->setTaskLists(tasks);
    m_agent.reconfigure();

    m_config.group("services").writeEntry("Tasks", 1);
    m_config.sync();

    emitResult();
}
