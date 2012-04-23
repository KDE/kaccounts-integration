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

#ifndef REMOVE_CALENDAR_AND_TASKS_H
#define REMOVE_CALENDAR_AND_TASKS_H

#include <KJob>
#include <KConfigGroup>

class RemoveCalendar : public KJob
{
    Q_OBJECT
    public:
        explicit RemoveCalendar(KConfigGroup group, QObject* parent);
        virtual ~RemoveCalendar();

        virtual void start();

    protected Q_SLOTS:
        void deleteCalendar();
        void removeResource();
        void resourceRemoved();

    private Q_SLOTS:
        void removeCalendarsInResource();

    private:
        KConfigGroup m_config;
};

#endif //REMOVE_CALENDAR_AND_TASKS_H