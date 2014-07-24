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
#include <libkgapi/common.h>
#include <libkgapi/account.h>

#include <QWizardPage>

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
        void error(KGAPI::Error, QString);
        void authenticated(KGAPI::Account::Ptr& acc);

    private:
        bool accountExists();
        void getTokenForAccount();

    private:
        bool    m_valid;
        GoogleWizard *m_wizard;
};

#endif //GOOGLEOAUTH_H
