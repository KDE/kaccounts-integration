/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
 *  Copyright (C) 2015 by Martin Klapetek <mklapetek@kde.org>                        *
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

#ifndef OSERVICES_H
#define OSERVICES_H

#include "ui_oservices.h"
#include <QWizardPage>

class OServices : public QWizardPage, Ui::Services
{
    Q_OBJECT

public:
    explicit OServices(QWizard *wizard);
    virtual ~OServices();

    virtual void initializePage();
    virtual bool validatePage();

private Q_SLOTS:
    void optionToggled(bool checked);

private:
    void addOption(const QString &id, const QString &displayText);

private:
    QWizard *m_wizard;
    QStringList m_disabledServices;
};

#endif //OSERVICES_H
