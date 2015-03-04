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

#include "accountwidget.h"
#include "models/accountsmodel.h"
#include "uipluginsmanager.h"
#include <core.h>
#include <kaccountsuiplugin.h>

#include <QDebug>
#include <QCheckBox>
#include <QPushButton>

#include <KLocalizedString>

#include <Accounts/Account>
#include <Accounts/Service>
#include <Accounts/Manager>

AccountWidget::AccountWidget(Accounts::Account *account, QWidget *parent)
    : QWidget(parent)
    , m_manager(KAccounts::accountsManager())
{
    setupUi(this);

    if (!account) {
        return;
    }

    setAccount(account);
}

AccountWidget::~AccountWidget()
{
    qDeleteAll(m_checkboxes);
}

void AccountWidget::setAccount(Accounts::Account *account)
{
    if (m_account) {
        disconnect(m_account.data(), 0, this, 0);
    }

    qDeleteAll(m_checkboxes);
    m_checkboxes.clear();
    QObject::disconnect(m_connection);

    QLayoutItem *child;
    while ((child = d_layout->takeAt(0)) != 0) {
        if (child->layout() != 0) {
            QLayoutItem *child2;
            while ((child2 = child->layout()->takeAt(0)) != 0) {
                delete child2->widget();
                delete child2;
            }
        }
        delete child;
    }

    QCheckBox *checkbox = 0;
    Accounts::ServiceList services = account->services();
    Q_FOREACH(const Accounts::Service &service, services) {
        // first select the service we'll be manipulating (enable/disable)
        account->selectService(service);
        checkbox = new QCheckBox(i18nd(service.trCatalog().toLatin1().constData(), service.displayName().toUtf8().constData()), this);
        // if there is a previously selected service (two lines above),
        // the account->enabled() is about the service, not the account
        checkbox->setChecked(account->enabled());
        checkbox->setProperty("service", service.name());

        KAccountsUiPlugin *uiPlugin = KAccounts::UiPluginsManager::pluginForService(service.serviceType());
        if (uiPlugin) {
            m_connection = connect(uiPlugin, &KAccountsUiPlugin::uiReady, [=]() {
                uiPlugin->showConfigureAccountDialog(m_account.data()->id());
            });
        }

        if (uiPlugin != 0) {
            QHBoxLayout *hlayout = new QHBoxLayout();
            QPushButton *imConfigButton = new QPushButton(i18n("Configure..."));
            connect(imConfigButton, &QPushButton::pressed, [=](){
                uiPlugin->init(KAccountsUiPlugin::ConfigureAccountDialog);
            });

            hlayout->addWidget(checkbox);
            hlayout->addWidget(imConfigButton);
            d_layout->addLayout(hlayout);
        } else {
            d_layout->addWidget(checkbox);
        }
        connect(checkbox, SIGNAL(clicked(bool)), SLOT(serviceChanged(bool)));
        m_checkboxes.insert(service.name(), checkbox);
    }

    m_account = account;
    connect(account, SIGNAL(enabledChanged(QString,bool)), SLOT(serviceEnabledChanged(QString,bool)));
}

void AccountWidget::serviceEnabledChanged(const QString &serviceName, bool enabled)
{
    if (serviceName.isEmpty()) {
        return;
    }

    Q_ASSERT(m_checkboxes.contains(serviceName));
    m_checkboxes[serviceName]->setChecked(enabled);
}

void AccountWidget::serviceChanged(bool enabled)
{
    QString service = sender()->property("service").toString();
    qDebug() << "Enabling: " << service << " enabled";
    if (!m_account) {
        return;
    }
    m_account.data()->selectService(m_manager->service(service));
    m_account.data()->setEnabled(enabled);
    m_account.data()->sync();
}
