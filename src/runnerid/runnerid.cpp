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

#include "runnerid.h"
#include "pages/rcredentials.h"
#include "pages/rservices.h"

#include <klocalizedstring.h>
#include <kpushbutton.h>
#include <kstandardguiitem.h>

#include <KWallet/Wallet>

using namespace KWallet;
RunnerIDWizard::RunnerIDWizard(QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags)
{
    setWindowTitle(i18n("Runner-id Wizad"));

    RCredentials *credentials = new RCredentials(this);
    RServices *services = new RServices(this);

    addPage(credentials);
    addPage(services);

    setButton(QWizard::BackButton, new
    KPushButton(KStandardGuiItem::back(KStandardGuiItem::UseRTL)));
    setButton(QWizard::NextButton, new
    KPushButton(KStandardGuiItem::forward(KStandardGuiItem::UseRTL)));
    setButton(QWizard::FinishButton, new KPushButton(KStandardGuiItem::apply()));
    setButton(QWizard::CancelButton, new KPushButton(KStandardGuiItem::cancel()));

    //We do not want "Forward" as text
    setButtonText(QWizard::NextButton, i18nc("Action to go to the next page on the wizard",
"Next"));
    setButtonText(QWizard::FinishButton, i18nc("Action to finish the wizard", "Finish"));
}

RunnerIDWizard::~RunnerIDWizard()
{

}

void RunnerIDWizard::done(int result)
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

    if (!wallet->isOpen() || !wallet->isEnabled()) {
        return;
    }

    wallet->createFolder("WebAccounts");
    wallet->setFolder("WebAccounts");

    wallet->writePassword("runnerid-" + m_username, m_password);
    wallet->sync();
    wallet->deleteLater();

    KConfigGroup config =
    KSharedConfig::openConfig("webaccounts")->group("accounts").group("runnerid").group(m_username);
    config.writeEntry("type", "runnerid");
    config.writeEntry("server", "http://runners-id.org/myowncloud/public/");
    KConfigGroup group = config.group("services");
    QStringList keys = m_services.keys();
    Q_FOREACH(const QString &key, keys) {
        group.writeEntry(key, m_services[key]);
    }

    Q_EMIT newAccount("runnerid", m_username);

    QWizard::done(result);
}

void RunnerIDWizard::setUsername(const QString& username)
{
    m_username = username;
}

void RunnerIDWizard::setPassword(const QString& password)
{
    m_password = password;
}

const QString RunnerIDWizard::username() const
{
    return m_username;
}

const QString RunnerIDWizard::password() const
{
    return m_password;
}

void RunnerIDWizard::activateOption(const QString& name, bool checked )
{
    if (!checked) {
        m_services[name] = 0;
        return;
    }

    m_services[name] = 2;
}