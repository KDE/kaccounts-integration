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
#include "google/google.h"

Services::Services(GoogleWizard* parent)
 : QWizardPage(parent)
 , m_wizard(parent)
{
    setupUi(this);
    setTitle(i18n("Select services"));
}

Services::~Services()
{

}

void Services::initializePage()
{
    QList <QWizard::WizardButton> list;
    list << QWizard::Stretch;
    list << QWizard::FinishButton;
    m_wizard->setButtonLayout(list);

    addOption("EMail", i18n("EMail"));
    addOption("Calendar", i18n("Calendar"));
    addOption("Contact", i18n("Contacts"));
    addOption("Tasks", i18n("Tasks"));
    addOption("Chat", i18n("Chat"));
}

void Services::addOption(const QString& text, const QString& displayText)
{
    ServiceOption *option = new ServiceOption(text, displayText, this);
    option->setBinary(true);

    connect(option, SIGNAL(toggled(QString, bool)), this, SLOT(optionToggled(QString, bool)));

    m_wizard->activateOption(text, true);

    d_layout->addWidget(option);
}

void Services::optionToggled(const QString& name, bool checked)
{
    m_wizard->activateOption(name, checked);
}