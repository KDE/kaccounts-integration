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

#include "oremovecontact.h"
#include "removeakonadiresource.h"
#include "davGroupware_settings.h"

ORemoveContact::ORemoveContact(KConfigGroup group, QObject* parent)
 : KJob(parent)
 , m_config(group)
{
    setObjectName(m_config.name());
    setProperty("type", QVariant::fromValue(m_config.readEntry("type")));
}

ORemoveContact::~ORemoveContact()
{

}

void ORemoveContact::start()
{
    QMetaObject::invokeMethod(this, "deleteContact", Qt::QueuedConnection);
}

void ORemoveContact::deleteContact()
{
    KConfigGroup services = m_config.group("services");
    if (services.readEntry("Calendar", 0) == 0) {
        removeResource();
        return;
    }

    removeCalendarsInResource();
}

void ORemoveContact::removeResource()
{
    RemoveAkonadiResource *remove = new RemoveAkonadiResource("contactAndCalendarResource", m_config, this);
    connect(remove, SIGNAL(finished(KJob*)), this, SLOT(resourceRemoved()));
    remove->start();
}

void ORemoveContact::removeCalendarsInResource()
{
    org::kde::Akonadi::davGroupware::Settings *settings = new org::kde::Akonadi::davGroupware::Settings(m_config.group("private").readEntry("contactAndCalendarResource"), "/Settings", QDBusConnection::sessionBus());

    QStringList list = settings->remoteUrls().value();
    Q_FOREACH(const QString &url, list) {
        if (url.contains("CardDav")) {
            list.removeOne(url);
        }
    }
    qDebug() << "Updated list: " << list;
    settings->setRemoteUrls(list);

    settings->writeConfig();

    if (m_config.group("services").readEntry("Calendar", 0) == 0) {
        removeResource();
        return;
    }

    resourceRemoved();
}

void ORemoveContact::resourceRemoved()
{
    m_config.group("services").writeEntry("Contact", 0);
    emitResult();
}

