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

#include <libkgapi2/types.h>
#include <libkgapi2/object.h>
#include <libkgapi2/account.h>
#include <libkgapi2/calendar/calendar.h>
#include <libkgapi2/calendar/calendarfetchjob.h>

#include <akonadi/agenttype.h>
#include <akonadi/agentmanager.h>
#include <akonadi/agentinstancecreatejob.h>

#include <KDebug>
#include <kwallet.h>
#include <klocalizedstring.h>
#include <set>

using namespace KWallet;
using namespace Akonadi;
using namespace KGAPI2;

CreateCalendar::CreateCalendar(KConfigGroup group, QObject* parent)
: KJob(parent)
, m_config(group)
{

}

CreateCalendar::~CreateCalendar()
{
}

void CreateCalendar::start()
{
    m_config.sync();
    QMetaObject::invokeMethod(this, "openKWallet", Qt::QueuedConnection);
}

KConfigGroup CreateCalendar::config() const
{
    return m_config;
}

void CreateCalendar::openKWallet()
{
    m_wallet = Wallet::openWallet(Wallet::LocalWallet(), 0);
    if (!m_wallet || !m_wallet->isEnabled() || !m_wallet->isOpen()) {
        finishWithError();
        return;
    }

    if (!m_wallet->hasFolder(QLatin1String("Akonadi Google"))) {
        finishWithError();
        return;
    }

    if (!m_wallet->setFolder(QLatin1String("Akonadi Google"))) {
        finishWithError();
        return;
    }
    if (!m_wallet->hasEntry(m_config.name())) {
        finishWithError();
        return;
    }

    if (m_wallet->readMap(m_config.name(), m_accInfo) != 0) {
        finishWithError();
        return;
    }

    if (m_config.hasGroup("private")  && m_config.group("private").hasKey("calendarAndTasksResource")) {
        QMetaObject::invokeMethod(this, "useTaskResource", Qt::QueuedConnection);
        return;
    }
    QMetaObject::invokeMethod(this, "createResource", Qt::QueuedConnection);
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
        finishWithError();
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
    AccountPtr acc(new Account);
    acc->setAccountName(m_config.name());
    acc->setAccessToken(m_accInfo[QLatin1String("accessToken")]);
    acc->setRefreshToken(m_accInfo[QLatin1String("refreshToken")]);

    qDebug() << acc->accessToken();
    qDebug() << acc->refreshToken();
    const QStringList scopes = m_accInfo[QLatin1String( "scopes" )].split( QLatin1Char(','), QString::SkipEmptyParts );
    QList<QUrl> scopeUrls;
    Q_FOREACH( const QString &scope, scopes ) {
        scopeUrls << QUrl( scope );
    }
    acc->setScopes(scopeUrls);

    CalendarFetchJob *job = new CalendarFetchJob(acc, this);
    connect(job, SIGNAL(finished(KGAPI2::Job*)), SLOT(replyReceived(KGAPI2::Job*)));
}

void CreateCalendar::useTaskResource()
{
    m_calendarSettings = new org::kde::Akonadi::GoogleCalendar::Settings(m_config.group("private").readEntry("calendarAndTasksResource"), "/Settings", QDBusConnection::sessionBus());

    QString id = m_config.group("private").readEntry("calendarAndTasksResource").remove("org.freedesktop.Akonadi.Resource.");
    m_agent = AgentManager::self()->instance(id);

    fetchDefaultCollections();
}

void CreateCalendar::replyReceived(KGAPI2::Job* job)
{
    CalendarFetchJob *cJob = qobject_cast<CalendarFetchJob*>(job);
    if (!cJob || cJob->error()) {
        finishWithError();
        return;
    }

    ObjectsList objects = cJob->items();
    QStringList calendars;

    Q_FOREACH(ObjectPtr object, objects) {
        CalendarPtr calendar = object.dynamicCast<Calendar>();
        calendars.append(calendar->uid());
    }

    m_calendarSettings->setCalendars(calendars);
    m_calendarSettings->writeConfig();
    m_agent.reconfigure();

    m_config.group("services").writeEntry("Calendar", 1);
    m_config.sync();
    m_agent.synchronize();

    emitResult();
}

void CreateCalendar::finishWithError()
{
    qDebug() << "finishing with error";
    setError(-1);
    m_config.group("services").writeEntry("Calendar", -1);
    m_config.sync();
    emitResult();
    return;
}