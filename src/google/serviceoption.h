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


#ifndef SERVICEOPTION_H
#define SERVICEOPTION_H

#include "ui_serviceoption.h"

#include <QWidget>

class ServiceOption : public QWidget
, Ui::ServiceOption
{
Q_OBJECT

public:
    ServiceOption(const QString& name, const QString& displayText, QWidget* parent = 0);

public Q_SLOTS:
    void setToggled(bool checked);

Q_SIGNALS:
    void toggled(const QString &name, bool);
};

#endif // SERVICEOPTION_H