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

#include "google.h"

#include "credentials.h"
#include "oauth.h"
#include "services.h"

#include <QtCore/QDebug>
#include <QtGui/QApplication>

#include <kwallet.h>
#include <kpushbutton.h>
#include <kstandardguiitem.h>

using namespace KWallet;

GoogleWizard::GoogleWizard(QWidget* parent) : QWizard(parent)
{
    Credentials *credentialsPage = new Credentials(this);
    OAuth *oauth = new OAuth(this);
    Services *services = new Services(this);

    addPage(credentialsPage);
    addPage(oauth);
    addPage(services);

    setButton(QWizard::BackButton, new KPushButton(KStandardGuiItem::back(KStandardGuiItem::UseRTL)));
    setButton(QWizard::NextButton, new KPushButton(KStandardGuiItem::forward(KStandardGuiItem::UseRTL)));
    setButton(QWizard::FinishButton, new KPushButton(KStandardGuiItem::apply()));
    setButton(QWizard::CancelButton, new KPushButton(KStandardGuiItem::cancel()));

    //We do not want "Forward" as text
    setButtonText(QWizard::NextButton, i18nc("Action to go to the next page on the wizard", "Next"));
    setButtonText(QWizard::FinishButton, i18nc("Action to finish the wizard", "Finish"));
}

GoogleWizard::~GoogleWizard()
{

}

void GoogleWizard::done(int result)
{
    if (result != 1) {
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
    wallet->writePassword(m_username, m_password);
    wallet->sync();
    wallet->deleteLater();

    KConfigGroup config = KSharedConfig::openConfig("webaccounts")->group("accounts").group("google").group(m_username);

    config.writeEntry("type", "google");

    KConfigGroup group = config.group("services");
    QStringList keys = m_services.keys();
    Q_FOREACH(const QString &key, keys) {
        group.writeEntry(key, m_services[key]);
    }

    Q_EMIT newAccount("google", m_username);

    QWizard::done(result);
}


void GoogleWizard::setUsername(const QString& username)
{
    m_username = username;
}

void GoogleWizard::setPassword(const QString& password)
{
    m_password = password;
}

const QString GoogleWizard::username() const
{
    return m_username;
}

const QString GoogleWizard::password() const
{
    return m_password;
}

void GoogleWizard::activateOption(const QString& name, bool checked )
{
    if (!checked) {
        m_services[name] = 0;
        return;
    }

    m_services[name] = 2;
}