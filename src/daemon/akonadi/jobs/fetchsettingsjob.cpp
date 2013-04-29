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

#include "fetchsettingsjob.h"
#include "dbussettingsinterfacejob.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QtXml/QDomDocument>

#include <kdebug.h>

FetchSettingsJob::FetchSettingsJob(QObject* parent)
: AbstractAkonadiJob(parent)
 , m_watcher(0)
{

}

FetchSettingsJob::~FetchSettingsJob()
{
    delete m_watcher;
}

void FetchSettingsJob::start()
{
    kDebug();
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void FetchSettingsJob::init()
{
    kDebug();
    if (m_interface.isEmpty()) {
        Q_ASSERT_X(!m_resourceId.isEmpty(), "Asking for the dbus itnerface", "Resource id can't be empty");
        DBusSettingsInterfaceJob *job = new DBusSettingsInterfaceJob(this);
        connect(job, SIGNAL(finished(KJob*)), SLOT(dbusSettingsPathDone(KJob*)));
        job->setResourceId(m_resourceId);
        job->start();
        return;
    }

    fetchSettings();
}

void FetchSettingsJob::dbusSettingsPathDone(KJob* job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    DBusSettingsInterfaceJob *dbusJob = qobject_cast<DBusSettingsInterfaceJob*>(job);
    m_interface = dbusJob->interface();

    fetchSettings();
}

void FetchSettingsJob::fetchSettings()
{
    Q_ASSERT_X(!m_interface.isEmpty(), "Fetching settings", "We need Interface to be set");
    Q_ASSERT_X(!m_resourceId.isEmpty(), "Fetching settings", "We need resource Id to be set");
    Q_ASSERT_X(!m_key.isEmpty(), "Fetching settings", "We need the key to be set");

    QString service = "org.freedesktop.Akonadi.Resource." + m_resourceId;
    QString path = "/Settings";

    QDBusMessage msg = QDBusMessage::createMethodCall(service, path, m_interface, m_key);

    QDBusInterface resource(service, path, m_interface);
    QDBusPendingCall reply = resource.asyncCall(m_key);

    m_watcher = new QDBusPendingCallWatcher(reply, this);
    connect(m_watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(fetchDone(QDBusPendingCallWatcher*)));
}

void FetchSettingsJob::fetchDone(QDBusPendingCallWatcher* watcher)
{
    if (watcher->isError()) {
        setError(-1);
    }

    emitResult();
}

QString FetchSettingsJob::key() const
{
    return m_key;
}

void FetchSettingsJob::setKey(const QString& key)
{
    m_key = key;
}