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

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QStackedLayout>
#include <QtGui/QListView>

#include <KDebug>
#include <kpluginfactory.h>
#include <kstandarddirs.h>


K_PLUGIN_FACTORY(WebAccountsFactory, registerPlugin<WebAccounts>();)
K_EXPORT_PLUGIN(WebAccountsFactory("webaccounts", "webaccounts"))

WebAccounts::WebAccounts(QWidget *parent, const QVariantList&)
: KCModule(WebAccountsFactory::componentData(), parent)
, m_create(0)
, m_layout(new QStackedLayout)
{

    m_ui = new Ui::KCMWebAccounts();
    m_ui->setupUi(this);

    m_ui->accountInfo->setLayout(m_layout);

    m_model = new AccountsModel(this);
    m_ui->accountsView->setIconSize(QSize(32,32));
    m_ui->accountsView->setModel(m_model);

    m_selectionModel = new QItemSelectionModel(m_model);
    m_selectionModel->setCurrentIndex(m_model->index(0), QItemSelectionModel::SelectCurrent);

    m_ui->accountsView->setSelectionModel(m_selectionModel);
    connect(m_ui->removeBtn, SIGNAL(clicked(bool)), this, SLOT(rmBtnClicked()));
    connect(m_ui->addBtn, SIGNAL(clicked(bool)), this, SLOT(addBtnClicked()));

    m_create = new Create(this);
    m_layout->addWidget(m_create->widget());
}

WebAccounts::~WebAccounts()
{
    delete m_ui;
}

void WebAccounts::addBtnClicked()
{

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
