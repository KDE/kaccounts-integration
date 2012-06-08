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

#include "wallet.h"
#include <webaccounts.h>

#include <KWallet/Wallet>
using namespace KWallet;

WalletPage::WalletPage(QWizard* parent)
: QWizardPage()
, m_wizard(parent)
, m_valid(false)
{
    setupUi(this);

    prepareWallet();
    connect(reloadWallet, SIGNAL(clicked(bool)), this, SLOT(prepareWallet()));
}

WalletPage::~WalletPage()
{

}

void WalletPage::initializePage()
{
    if (!m_valid) {
        return;
    }

    QMetaObject::invokeMethod(m_wizard, "next", Qt::QueuedConnection);
}

bool WalletPage::isComplete() const
{
    return m_valid;
}

#include <QDebug>
void WalletPage::prepareWallet()
{
    Wallet *wallet = WebAccounts::wallet();
    if (!wallet) {
        return;
    }

    Q_EMIT completeChanged();
    m_valid = true;
    m_wizard->next();
}

bool WalletPage::validatePage()
{
    return m_valid;
}
