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

#include "fservices.h"
#include "facebook.h"
#include "google/pages/serviceoption.h"

FServices::FServices(FacebookWizard* wizard)
 : QWizardPage()
 , m_wizard(wizard)
{
    setupUi(this);
}

FServices::~FServices()
{

}

void FServices::initializePage()
{
    addOption("Chat", i18n("Chat"));
    addOption("PIM", i18n("Contacts and Events"));
}

void FServices::addOption(const QString& text, const QString& displayText)
{
    ServiceOption *option = new ServiceOption(text, displayText, this);
    connect(option, SIGNAL(toggled(QString, bool)), this, SLOT(optionToggled(QString, bool)));

    m_wizard->activateOption(text, true);

    d_layout->addWidget(option);
}

void FServices::optionToggled(const QString& name, bool checked)
{
    m_wizard->activateOption(name, checked);
}
