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

#ifndef OSERVICES_H
#define OSERVICES_H

#include "ui_services.h"
#include <QWizardPage>

class OwnCloudWizard;
class OServices : public QWizardPage, Ui::Services
{
    Q_OBJECT
    public:
        explicit OServices(OwnCloudWizard *wizard);
        virtual ~OServices();

        virtual void initializePage();

    private Q_SLOTS:
        void optionToggled(const QString &name, bool checked);

    private:
        void addOption(const QString& text, const QString& displayText);

    private:
        OwnCloudWizard *m_wizard;
};

#endif //OSERVICES_H