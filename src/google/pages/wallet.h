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

#ifndef WALLET_PAGE_H
#define WALLET_PAGE_H

#include "ui_wallet.h"

#include <libkgoogle/common.h>
#include <libkgoogle/account.h>

#include <QtGui/QWizardPage>

class QWizard;
class WalletPage : public QWizardPage, Ui::WalletGui
{
Q_OBJECT
    public:
        WalletPage(QWizard *parent);
        virtual ~WalletPage();

        virtual bool validatePage();
        virtual void initializePage();
        virtual bool isComplete() const;

    private Q_SLOTS:
        void prepareWallet();

    private:
        bool    m_valid;
        QWizard *m_wizard;
};

#endif //WALLET_PAGE_H