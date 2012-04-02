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

#ifndef CREATECALENDAR_H
#define CREATECALENDAR_H

#include <QtCore/QString>

#include <kjob.h>
#include <kconfiggroup.h>

#include <akonadi/agentinstance.h>

namespace KGoogle {
    class Reply;
};
using namespace Akonadi;
class KConfigGroup;
class OrgKdeAkonadiGoogleCalendarSettingsInterface;

class CreateCalendar : public KJob
{
    Q_OBJECT
    public:
        explicit CreateCalendar(KConfigGroup &group, QObject* parent = 0);
        virtual ~CreateCalendar();

        virtual void start();

    protected:
        void configureAccountName(const QString &service);
        void fetchDefaultCollections();

    protected Q_SLOTS:
        void useTaskResource();
        void createResource();
        void resourceCreated(KJob *job);
        void replyReceived(KGoogle::Reply *reply);

    protected:
        OrgKdeAkonadiGoogleCalendarSettingsInterface *m_calendarSettings;
        KConfigGroup m_config;
        AgentInstance m_agent;
};

#endif //CREATECALENDAR_H