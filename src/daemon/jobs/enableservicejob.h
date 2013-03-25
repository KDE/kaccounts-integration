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

#ifndef ENABLE_SERVICE_JOB_H
#define ENABLE_SERVICE_JOB_H

#include <Accounts/Account>

#include <kjob.h>

class EnableServiceJob : public KJob
{
    Q_OBJECT
    public:
        enum Status {
            Enable = 1,
            Disable
        };
        explicit EnableServiceJob(QObject* parent = 0);
        virtual void start();

        QString interface() const;
        void setInterface(const QString &interface);

        QString resourceId() const;
        void setResourceId(const QString &resourceId);

        QString service();
        void setService(const QString &serviceName, Status status);

    public Q_SLOTS:
        void init();
        void fetchSettingsJobDone(KJob* job);
        void changeSettingsDone(KJob* job);

    private:
        QString m_service;
        QString m_interface;
        QString m_resourceId;
        Status m_serviceStatus;
};

#endif //ENABLE_SERVICE_JOB_H
