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

#include "serviceoption.h"

ServiceOption::ServiceOption(const QString& checkboxText, QWidget* parent): QWidget(parent)
{
    setupUi(this);

    checkBox->setText(checkboxText);

    connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
}

void ServiceOption::setChecked(bool checked)
{
    checkBox->setChecked(checked);
}

void ServiceOption::toggled(bool checked)
{
    if (checked == true) {
//         emit selected(m_service);
    }
}
