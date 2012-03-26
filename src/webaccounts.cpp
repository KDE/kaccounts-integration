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

#include "webaccounts.h"
#include "ui_kcm.h"
#include "ui_types.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>

#include <kpluginfactory.h>

K_PLUGIN_FACTORY(WebAccountsFactory, registerPlugin<WebAccounts>();)
K_EXPORT_PLUGIN(WebAccountsFactory("webaccounts", "webaccounts"))

WebAccounts::WebAccounts(QWidget *parent, const QVariantList&) : KCModule(WebAccountsFactory::componentData(), parent)
{

    m_ui = new Ui::KCMWebAccounts();
    m_ui->setupUi(this);

    m_newAccountItem = new QListWidgetItem(i18n("New Account"), m_ui->accList);
    m_newAccountItem->setIcon(QIcon::fromTheme("applications-education-miscellaneous"));
    m_ui->accList->setIconSize(QSize(64, 64));
    m_ui->accList->addItem(m_newAccountItem);

    connect(m_ui->addBtn, SIGNAL(clicked(bool)), this, SLOT(addBtnClicked()));
    connect(m_ui->accList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(currentItemChanged(QListWidgetItem*,QListWidgetItem*)));

    m_ui->accList->setCurrentItem(m_newAccountItem);
}

WebAccounts::~WebAccounts()
{
    delete m_ui;
}

void WebAccounts::addBtnClicked()
{
    m_ui->accList->setCurrentItem(m_newAccountItem, QItemSelectionModel::SelectCurrent);
}

void WebAccounts::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current == m_newAccountItem) {
        m_ui->accountInfo->setTitle(i18n("Select a supported Web Account"));
        Ui::createForm *form = new Ui::createForm();

        QWidget *widget = new QWidget();
        form->setupUi(widget);

        QVBoxLayout *vbox = new QVBoxLayout(m_ui->accountInfo);
        vbox->addWidget(widget);
    }
}

#include "webaccounts.moc"
