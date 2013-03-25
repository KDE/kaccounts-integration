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

#include "dbussettingspathjob.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>
#include <QtXml/QDomDocument>

#include <kdebug.h>

DBusSettingsPathJob::DBusSettingsPathJob(QObject* parent) : KJob(parent)
{

}

void DBusSettingsPathJob::start()
{
    kDebug();
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void DBusSettingsPathJob::init()
{
    kDebug();
    QString service = "org.freedesktop.Akonadi.Resource." + m_resourceId;
    QString path = "/Settings";
    QString interface = "org.freedesktop.DBus.Introspectable";

    QDBusMessage msg = QDBusMessage::createMethodCall(service, path, interface, "Introspect");

    QDBusPendingCall reply = QDBusConnection::sessionBus().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(introspectDone(QDBusPendingCallWatcher*)));
}

QString DBusSettingsPathJob::path() const
{
    return m_path;
}

void DBusSettingsPathJob::setResourceId(const QString& resourceId)
{
    m_resourceId = resourceId;
}

void DBusSettingsPathJob::introspectDone(QDBusPendingCallWatcher* watcher)
{
    kDebug();
    QDBusPendingReply<QString> reply = *watcher;
    if (reply.isError()) {
        kDebug() << reply.error().message();
        kDebug() << reply.error().name();

        setError(reply.error().type());
        setErrorText(reply.error().name());
        watcher->deleteLater();
        emitResult();
        return;
    }

    QDomDocument dom;
    QDomElement node;
    QString interfaceName;
    QString akonadi("org.kde.Akonadi.");
    QString settings(".Settings");
    dom.setContent(reply.value());
    watcher->deleteLater();

    QDomNodeList nodeList = dom.documentElement().elementsByTagName("interface");
    for (int i = 0; i < nodeList.count(); i++) {
        node = nodeList.item(i).toElement();
        if (node.isNull() || !node.hasAttribute("name")) {
            continue;
        }
        interfaceName = node.attribute("name");
        if (!interfaceName.startsWith(akonadi) || !interfaceName.endsWith(settings)) {
            continue;
        }

        m_path = interfaceName.remove(akonadi).remove(settings);
        break;
    }

    if (m_path.isEmpty()) {
        setError(-1);
        setErrorText("Settings Path name not found");
        emitResult();
        return;
    }

    kDebug() << "Resource: " << m_path;

    emitResult();
}