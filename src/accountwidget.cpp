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

#include <QtCore/QDebug>
#include <QtGui/QCheckBox>

#include <Accounts/Account>
#include <Accounts/Service>
#include <Accounts/Manager>

#include <KDebug>

AccountWidget::AccountWidget(Accounts::Account *account, QWidget* parent)
 : QWidget(parent)
 , m_manager(new Accounts::Manager(this))
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
    delete m_manager;
}

void AccountWidget::setAccount(Accounts::Account* account)
{
    if (m_account) {
        disconnect(m_account.data(), 0, this, 0);
    }

    qDeleteAll(m_checkboxes);
    m_checkboxes.clear();

    QCheckBox *checkbox = 0;
    Accounts::ServiceList services = account->services();
    Q_FOREACH(const Accounts::Service &service, services) {
        account->selectService(service);
        checkbox = new QCheckBox(service.displayName(), this);
        checkbox->setChecked(account->enabled());
        checkbox->setProperty("service", service.name());
        d_layout->addWidget(checkbox);
        connect(checkbox, SIGNAL(clicked(bool)), SLOT(serviceChanged(bool)));
        m_checkboxes.insert(service.name(), checkbox);
    }

    m_account = account;
    connect(account, SIGNAL(enabledChanged(QString,bool)), SLOT(serviceEnabledChanged(QString,bool)));
}

void AccountWidget::serviceEnabledChanged(const QString& serviceName, bool enabled)
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
    kDebug() << "Enabling: " << service << " enabled";
    if (!m_account) {
        return;
    }
    m_account.data()->selectService(m_manager->service(service));
    m_account.data()->setEnabled(enabled);
    m_account.data()->sync();
}