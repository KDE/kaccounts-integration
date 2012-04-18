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

#include "oremovecalendar.h"
#include "removeakonadiresource.h"
#include "davGroupware_settings.h"

ORemoveCalendar::ORemoveCalendar(KConfigGroup group, QObject* parent)
 : KJob(parent)
 , m_config(group)
{
    setObjectName(m_config.name());
    setProperty("type", QVariant::fromValue(m_config.readEntry("type")));
}

ORemoveCalendar::~ORemoveCalendar()
{

}

void ORemoveCalendar::start()
{
    QMetaObject::invokeMethod(this, "deleteCalendar", Qt::QueuedConnection);
}

void ORemoveCalendar::deleteCalendar()
{
    KConfigGroup services = m_config.group("services");
    if (services.readEntry("Contact", 0) == 0) {
        removeResource();
        return;
    }

    removeCalendarsInResource();
}

void ORemoveCalendar::removeResource()
{
    RemoveAkonadiResource *remove = new RemoveAkonadiResource("contactAndCalendarResource", m_config, this);
    connect(remove, SIGNAL(finished(KJob*)), this, SLOT(resourceRemoved()));
    remove->start();
}

void ORemoveCalendar::removeCalendarsInResource()
{
    org::kde::Akonadi::davGroupware::Settings *settings = new org::kde::Akonadi::davGroupware::Settings(m_config.group("private").readEntry("contactAndCalendarResource"), "/Settings", QDBusConnection::sessionBus());

    QStringList list = settings->remoteUrls().value();
    Q_FOREACH(const QString &url, list) {
        if (url.contains("CalDav")) {
            list.removeOne(url);
        }
    }
    qDebug() << "Updated list: " << list;
    settings->setRemoteUrls(list);

    settings->writeConfig();

    if (m_config.group("services").readEntry("Contact", 0) == 0) {
        removeResource();
        return;
    }

    resourceRemoved();
}

void ORemoveCalendar::resourceRemoved()
{
    m_config.group("services").writeEntry("Calendar", 0);
    emitResult();
}

