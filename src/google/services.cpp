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

#include "services.h"
#include "serviceoption.h"

Services::Services(GoogleWizard* parent)
{
    setupUi(this);

    m_wizard = parent;
}

Services::~Services()
{

}

void Services::initializePage()
{
    d_layout->addWidget(new ServiceOption(i18n("EMail")));
    d_layout->addWidget(new ServiceOption(i18n("Calendar")));
    d_layout->addWidget(new ServiceOption(i18n("Contact")));
    d_layout->addWidget(new ServiceOption(i18n("Tasks")));
    d_layout->addWidget(new ServiceOption(i18n("Chat")));
}
