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

#include "owncloud.h"
#include "pages/basicinfo.h"
#include "pages/connecting.h"
#include "pages/oservices.h"

#include <klocalizedstring.h>
#include <kstandardguiitem.h>

#include <QPushButton>
#include <QWizard>

// #include <KWallet/Wallet>

// using namespace KWallet;

OwnCloudWizard::OwnCloudWizard(QWidget *parent)
    : KAccountsUiPlugin(parent)
{

}

OwnCloudWizard::~OwnCloudWizard()
{

}

void OwnCloudWizard::init(KAccountsUiPlugin::UiType type)
{
    if (type == KAccountsUiPlugin::NewAccountDialog) {
        m_wizard = new QWizard();
        m_wizard->setWindowTitle(i18n("Add ownCloud Account"));

        BasicInfo *basicInfo = new BasicInfo(m_wizard);
        Connecting *connecting = new Connecting(m_wizard);
        OServices *services = new OServices(m_wizard);

        m_wizard->addPage(basicInfo);
        m_wizard->addPage(connecting);
        m_wizard->addPage(services);

        QPushButton *backButton = new QPushButton(m_wizard);
        QPushButton *forwardButton = new QPushButton(m_wizard);
        QPushButton *applyButton = new QPushButton(m_wizard);
        QPushButton *cancelButton = new QPushButton(m_wizard);

        KGuiItem::assign(backButton, KStandardGuiItem::back());
        KGuiItem::assign(forwardButton, KStandardGuiItem::forward());
        KGuiItem::assign(applyButton, KStandardGuiItem::apply());
        KGuiItem::assign(cancelButton, KStandardGuiItem::cancel());

        m_wizard->setButton(QWizard::BackButton, backButton);
        m_wizard->setButton(QWizard::NextButton, forwardButton);
        m_wizard->setButton(QWizard::FinishButton, applyButton);
        m_wizard->setButton(QWizard::CancelButton, cancelButton);

        // We do not want "Forward" as text
        m_wizard->setButtonText(QWizard::NextButton, i18nc("Action to go to the next page on the wizard", "Next"));
        m_wizard->setButtonText(QWizard::FinishButton, i18nc("Action to finish the wizard", "Finish"));

        m_wizard->setOption(QWizard::NoDefaultButton, false);


        connect(m_wizard, &QWizard::accepted, this, &OwnCloudWizard::done);

        Q_EMIT uiReady();
    }

}

void OwnCloudWizard::setProviderName(const QString &providerName)
{
    //TODO: unused?
    m_providerName = providerName;
}

void OwnCloudWizard::showNewAccountDialog()
{
    if (m_wizard) {
        m_wizard->exec();
    }
}

void OwnCloudWizard::showConfigureAccountDialog(const quint32 accountId)
{

}

QStringList OwnCloudWizard::supportedServicesForConfig() const
{
    return QStringList();
}

void OwnCloudWizard::done()
{
    QVariantMap data;
    data.insert("server", m_wizard->property("server"));

    const QString username = m_wizard->property("username").toString();
    QUrl carddavUrl = m_wizard->property("server").toUrl();
    carddavUrl.setPath(carddavUrl.path() + QString("/remote.php/carddav/addressbooks/%1").arg(username));

    data.insert("carddavUrl", carddavUrl);
    Q_EMIT success(username, m_wizard->property("password").toString(), data);
}

void OwnCloudWizard::setUsername(const QString &username)
{
    m_username = username;
}

void OwnCloudWizard::setPassword(const QString &password)
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

void OwnCloudWizard::activateOption(const QString &name, bool checked)
{
    if (!checked) {
        m_services[name] = 0;
        return;
    }

    m_services[name] = 2;
}
