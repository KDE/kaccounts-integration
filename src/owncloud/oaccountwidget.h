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

#ifndef O_ACCOUNTWIDGET_H
#define O_ACCOUNTWIDGET_H

#include "ui_services.h"

#include <QtGui/QWidget>

#include <KConfigGroup>

class ServiceOption;
class OAccountWidget : public QWidget, Ui::Services
{
Q_OBJECT
    public:
        explicit OAccountWidget(KConfigGroup group, QWidget* parent);
        virtual ~OAccountWidget();

    public Q_SLOTS:
        void serviceChanged(const QString& service, bool enabled);

    private Q_SLOTS:
        void updateCalendar();
        void updateContact();
        void updateFile();

        void updateAll();

    private:
        void modifyCalendar(bool enabled);
        void modifyContact(bool enabled);
        void modifyFile(bool enabled);

        void updateService(const QString &name);
    private:
        KConfigGroup m_config;
        QHash<QString, ServiceOption*> m_serviceWidgets;
};

#endif //O_ACCOUNTWIDGET_H