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

#include "owncloud.h"
#include "pages/basicinfo.h"
#include "pages/connecting.h"
#include "pages/oservices.h"
#include "../google/pages/wallet.h"

#include <klocalizedstring.h>
#include <kstandardguiitem.h>

#include <QPushButton>

#include <KWallet/Wallet>

using namespace KWallet;
OwnCloudWizard::OwnCloudWizard(QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags)
{
    setWindowTitle(i18n("ownCloud account Wizard"));

    WalletPage *wallet = new WalletPage(this);
    BasicInfo *basicInfo = new BasicInfo(this);
    Connecting *connecting = new Connecting(this);
    OServices *services = new OServices(this);

    addPage(wallet);
    addPage(basicInfo);
    addPage(connecting);
    addPage(services);

    QPushButton *backButton;
    QPushButton *forwardButton;
    QPushButton *applyButton;
    QPushButton *cancelButton;

    KGuiItem::assign(backButton, KStandardGuiItem::back());
    KGuiItem::assign(forwardButton, KStandardGuiItem::forward());
    KGuiItem::assign(applyButton, KStandardGuiItem::apply());
    KGuiItem::assign(cancelButton, KStandardGuiItem::cancel());

    setButton(QWizard::BackButton, backButton);
    setButton(QWizard::NextButton, forwardButton);
    setButton(QWizard::FinishButton, applyButton);
    setButton(QWizard::CancelButton, cancelButton);

    //We do not want "Forward" as text
    setButtonText(QWizard::NextButton, i18nc("Action to go to the next page on the wizard", "Next"));
    setButtonText(QWizard::FinishButton, i18nc("Action to finish the wizard", "Finish"));
}

OwnCloudWizard::~OwnCloudWizard()
{

}

void OwnCloudWizard::done(int result)
{
    if (result != 1 || m_services.isEmpty()) {
        QWizard::done(result);
        return;
    }

    WId windowId = 0;
    if (qApp->activeWindow()) {
        windowId = qApp->activeWindow()->winId();
    }

    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), windowId, Wallet::Synchronous);

    if (!wallet || !wallet->isOpen() || !wallet->isEnabled()) {
        return;
    }

    wallet->createFolder("WebAccounts");
    wallet->setFolder("WebAccounts");

    wallet->writePassword("owncloud-" + m_username, m_password);
    wallet->sync();
    wallet->deleteLater();

    KConfigGroup config = KSharedConfig::openConfig("webaccounts")->group("accounts").group("owncloud").group(m_username);
    config.writeEntry("type", "owncloud");
    config.writeEntry("server", m_server.url());
    KConfigGroup group = config.group("services");
    QStringList keys = m_services.keys();
    Q_FOREACH(const QString &key, keys) {
        group.writeEntry(key, m_services[key]);
    }

    Q_EMIT newAccount("owncloud", m_username);

    QWizard::done(result);
}

void OwnCloudWizard::setUsername(const QString& username)
{
    m_username = username;
}

void OwnCloudWizard::setPassword(const QString& password)
{
    m_password = password;
}

void OwnCloudWizard::setServer(const QUrl &server)
{
    m_server = server;
}

const QString OwnCloudWizard::username() const
{
    return m_username;
}

const QString OwnCloudWizard::password() const
{
    return m_password;
}

const QUrl OwnCloudWizard::server() const
{
    return m_server;
}

void OwnCloudWizard::activateOption(const QString& name, bool checked )
{
    if (!checked) {
        m_services[name] = 0;
        return;
    }

    m_services[name] = 2;
}