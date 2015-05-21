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

#ifndef GOOGLECREDENTIALS_H
#define GOOGLECREDENTIALS_H

#include "ui_credentials.h"

#include <QWizardPage>

namespace KWallet {
    class Wallet;
};
class GoogleWizard;
class Credentials : public QWizardPage, Ui::Google
{
Q_OBJECT
    public:
        Credentials(GoogleWizard *parent);
        virtual ~Credentials();

        virtual void initializePage();
        virtual bool isComplete() const;
        virtual bool validatePage();

    public Q_SLOTS:
        void validateForm();

    private:
        bool accountExists(const QString &email);

    private:
        bool m_completed;
        GoogleWizard *m_wizard;
        KWallet::Wallet *m_wallet;
};

#endif //GOOGLECREDENTIALS_H