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


#include <accountwidget.h>

#include "google/serviceoption.h"

#include <QtCore/QDebug>

AccountWidget::AccountWidget(KConfigGroup group, QWidget* parent)
 : QWidget(parent)
 , m_config(group)
{
    setupUi(this);

    int status = 0;
    KConfigGroup services = m_config.group("services");
    QStringList keys = services.keyList();
    Q_FOREACH(const QString &key, keys) {
        status = services.readEntry(key, 0);
        ServiceOption *option = new ServiceOption(key, key, this);
        option->setStatus(status);
        connect(option, SIGNAL(toggled(QString,bool)), this, SLOT(serviceChanged(QString,bool)));
        d_layout->addWidget(option);
    }
}

AccountWidget::~AccountWidget()
{

}

void AccountWidget::serviceChanged(const QString& name, bool enabled)
{

}