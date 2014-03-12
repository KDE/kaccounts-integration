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

#ifndef GOOGLEOAUTH_H
#define GOOGLEOAUTH_H

#include "ui_oauth.h"
#include <libkgapi2/types.h>
#include <libkgapi2/account.h>

#include <QtGui/QWizardPage>

namespace KGAPI2 {
    class Job;
}
namespace KWallet {
    class Wallet;
}
class GoogleWizard;
class OAuth : public QWizardPage, Ui::OAuth
{
Q_OBJECT
    public:
        OAuth(GoogleWizard *parent);
        virtual ~OAuth();

        virtual bool validatePage();
        virtual void initializePage();
        virtual bool isComplete() const;

    private Q_SLOTS:
        void authenticated(KGAPI2::Job* job);

    private:
        void setError();
        bool accountExists();
        void getTokenForAccount();

    private:
        bool    m_valid;
        KWallet::Wallet *m_wallet;
        GoogleWizard *m_wizard;
};

#endif //GOOGLEOAUTH_H
