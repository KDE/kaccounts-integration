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
#include "create.h"
#include "ui_kcm.h"

#include "models/accountsmodel.h"
#include "models/modeltest.h"
#include "accountwidget.h"

#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QStackedLayout>
#include <QListView>

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(WebAccountsFactory, "kcm_kdeaccounts.json", registerPlugin<WebAccounts>();)

WebAccounts::WebAccounts(QWidget *parent, const QVariantList&)
    : KCModule(parent)
      , m_create(0)
      , m_layout(new QStackedLayout)
{
    m_ui = new Ui::KCMWebAccounts();
    m_ui->setupUi(this);

    m_ui->accountInfo->setLayout(m_layout);
    m_create = new Create(this);
    m_layout->addWidget(m_create->widget());

    m_accWidget = new AccountWidget(0, this);
    m_layout->addWidget(m_accWidget);

    m_model = new AccountsModel(this);
    m_selectionModel = new QItemSelectionModel(m_model);
    connect(m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(currentChanged(QModelIndex,QModelIndex)));
    m_selectionModel->setCurrentIndex(m_model->index(0), QItemSelectionModel::SelectCurrent);

    m_ui->accountsView->setIconSize(QSize(32,32));
    m_ui->accountsView->setModel(m_model);
    m_ui->accountsView->setSelectionModel(m_selectionModel);

    connect(m_ui->removeBtn, SIGNAL(clicked(bool)), this, SLOT(rmBtnClicked()));
    connect(m_ui->addBtn, SIGNAL(clicked(bool)), this, SLOT(addBtnClicked()));
}

void WebAccounts::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if (!current.isValid()) {
        return;
    }

    if (current.row() == m_model->rowCount() - 1) {
        m_ui->removeBtn->setDisabled(true);
        m_layout->setCurrentIndex(0);
        return;
    }

    Accounts::Account* acc = qobject_cast<Accounts::Account*>(m_model->data(current, AccountsModel::Data).value<QObject*>());
    m_accWidget->setAccount(acc);
    m_ui->removeBtn->setDisabled(false);
    m_layout->setCurrentIndex(1);
}

WebAccounts::~WebAccounts()
{
    delete m_ui;
}

void WebAccounts::addBtnClicked()
{
    m_selectionModel->setCurrentIndex(m_model->index(m_model->rowCount() - 1), QItemSelectionModel::SelectCurrent);
}

void WebAccounts::rmBtnClicked()
{
    QModelIndex index = m_selectionModel->currentIndex();
    if (!index.isValid()) {
        return;
    }
    m_model->removeRows(index.row(), 1);
}


#include "webaccounts.moc"
