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

#include "changesettingsjob.h"
#include "dbussettingsinterfacejob.h"

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QDBusPendingReply>

#include <KDebug>

ChangeSettingsJob::ChangeSettingsJob(QObject* parent): KJob(parent)
{
}

ChangeSettingsJob::~ChangeSettingsJob()
{

}

void ChangeSettingsJob::start()
{
    kDebug();
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void ChangeSettingsJob::init()
{
    if (m_interface.isEmpty()) {
        DBusSettingsInterfaceJob *job = new DBusSettingsInterfaceJob(this);
        connect(job, SIGNAL(finished(KJob*)), SLOT(dbusSettingsPath(KJob*)));
        job->setResourceId(m_resourceId);
        job->start();
        return;
    }

    setConfiguration();
}

void ChangeSettingsJob::dbusSettingsPath(KJob* job)
{
    DBusSettingsInterfaceJob *dbusJob = qobject_cast<DBusSettingsInterfaceJob*>(job);
    m_interface = dbusJob->interface();

    setConfiguration();
}

void ChangeSettingsJob::setAccountId(const Accounts::AccountId& accId)
{
    m_accountId = accId;
}

void ChangeSettingsJob::setResourceId(const QString& resourceId)
{
    m_resourceId = resourceId;
}

void ChangeSettingsJob::setSetting(const QString& key, const QVariant& value)
{
    m_key = key;
    m_value = value;
}

void ChangeSettingsJob::setConfiguration()
{
    kDebug();

    QDBusMessage msg = createCall(m_key);
    msg.setArguments(QVariantList() << m_value);

    QDBusPendingCall reply = QDBusConnection::sessionBus().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(accountSet(QDBusPendingCallWatcher*)));
}

void ChangeSettingsJob::accountSet(QDBusPendingCallWatcher* watcher)
{
    kDebug();
    QDBusPendingReply<void> reply = *watcher;
    if (reply.isError()) {
        setError(-1);
        setErrorText(reply.error().message());
        emitResult();
        return;
    } else {
        writeConfig();
    }

    watcher->deleteLater();
}

void ChangeSettingsJob::writeConfig()
{
    kDebug();
    QDBusPendingCall reply = QDBusConnection::sessionBus().asyncCall(createCall("writeConfig"));
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(configWritten(QDBusPendingCallWatcher*)));
}

void ChangeSettingsJob::configWritten(QDBusPendingCallWatcher* watcher)
{
    kDebug();
    watcher->deleteLater();

    QDBusPendingReply<void> reply = *watcher;
    if (reply.isError()) {
        setError(-1);
        setErrorText(reply.error().message());
    }

    emitResult();
}

QDBusMessage ChangeSettingsJob::createCall(const QString& method)
{
    QString service = "org.freedesktop.Akonadi.Resource." + m_resourceId;
    QString path = "/Settings";

    QDBusMessage msg = QDBusMessage::createMethodCall(service, path, m_interface, method);
    return msg;
}
