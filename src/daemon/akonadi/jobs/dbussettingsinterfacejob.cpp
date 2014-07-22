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

#include "dbussettingsinterfacejob.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>
#include <QDomDocument>
#include <QDebug>

DBusSettingsInterfaceJob::DBusSettingsInterfaceJob(QObject* parent) : AbstractAkonadiJob(parent)
{

}

void DBusSettingsInterfaceJob::start()
{
    qDebug();
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void DBusSettingsInterfaceJob::init()
{
    qDebug();
    QString service = "org.freedesktop.Akonadi.Resource." + m_resourceId;
    QString path = "/Settings";
    QString interface = "org.freedesktop.DBus.Introspectable";

    QDBusMessage msg = QDBusMessage::createMethodCall(service, path, interface, "Introspect");

    QDBusPendingCall reply = QDBusConnection::sessionBus().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(introspectDone(QDBusPendingCallWatcher*)));
}

void DBusSettingsInterfaceJob::introspectDone(QDBusPendingCallWatcher* watcher)
{
    qDebug();
    watcher->deleteLater();
    QDBusPendingReply<QString> reply = *watcher;
    if (reply.isError()) {
        qDebug() << reply.error().message();
        qDebug() << reply.error().name();

        setError(reply.error().type());
        setErrorText(reply.error().name());
        emitResult();
        return;
    }

    QDomDocument dom;
    QDomElement node;
    QString interfaceName;
    QString akonadi("org.kde.Akonadi.");
    QString settings(".Settings");
    dom.setContent(reply.value());

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

        m_interface = interfaceName;
        break;
    }

    if (m_interface.isEmpty()) {
        setError(-1);
        setErrorText("Settings Interface not found");
    }

    qDebug() << "Interface: " << m_interface;

    emitResult();
}