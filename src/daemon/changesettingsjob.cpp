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

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QDBusPendingReply>

#include <QtXml/QDomDocument>

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
    QMetaObject::invokeMethod(this, "getMethodName", Qt::QueuedConnection);
}

void ChangeSettingsJob::getMethodName()
{
    kDebug();
    QString service = "org.freedesktop.Akonadi.Resource." + m_agent.identifier();
    QString path = "/Settings";
    QString interface = "org.freedesktop.DBus.Introspectable";

    QDBusMessage msg = QDBusMessage::createMethodCall(service, path, interface, "Introspect");

    QDBusPendingCall reply = QDBusConnection::sessionBus().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(introspectDo(QDBusPendingCallWatcher*)));
}

void ChangeSettingsJob::setAccountId(const Accounts::AccountId& accId)
{
    m_accountId = accId;
}

void ChangeSettingsJob::setAgentInstance(const Akonadi::AgentInstance& agent)
{
    m_agent = agent;
}

void ChangeSettingsJob::setSetting(const QString& key, const QVariant& value)
{
    m_key = key;
    m_value = value;
}

void ChangeSettingsJob::introspectDo(QDBusPendingCallWatcher* watcher)
{
    kDebug();
    QDBusPendingReply<QString> reply = *watcher;
    if (reply.isError()) {
        kDebug();
        setError(reply.error().type());
        setErrorText(reply.error().name());
        watcher->deleteLater();
        emitResult();
        return;
    }

    kDebug() << reply.value();
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

        m_resource = interfaceName.remove(akonadi).remove(settings);
        break;
    }

    if (m_resource.isEmpty()) {
        setError(-1);
        setErrorText("Resource name not found");
        emitResult();
        return;
    }

    kDebug() << "Resource: " << m_resource;
    setAccountId();
}

void ChangeSettingsJob::setAccountId()
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
    QDBusPendingReply<void> reply = *watcher;
    if (!reply.isError()) {
        setError(-1);
        m_agent.reconfigure();
    } else {
        setErrorText(reply.error().message());
    }

    emitResult();
}

QDBusMessage ChangeSettingsJob::createCall(const QString& method)
{
    QString service = "org.freedesktop.Akonadi.Resource." + m_agent.identifier();
    QString path = "/Settings";
    QString interface = "org.kde.Akonadi.";
    interface.append(m_resource);
    interface.append(".Settings");

    QDBusMessage msg = QDBusMessage::createMethodCall(service, path, interface, method);
    return msg;
}
