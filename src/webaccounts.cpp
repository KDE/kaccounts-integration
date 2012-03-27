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

#include <QDebug>

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>

#include <kpluginfactory.h>

K_PLUGIN_FACTORY(WebAccountsFactory, registerPlugin<WebAccounts>();)
K_EXPORT_PLUGIN(WebAccountsFactory("webaccounts", "webaccounts"))

WebAccounts::WebAccounts(QWidget *parent, const QVariantList&)
: KCModule(WebAccountsFactory::componentData(), parent)
, m_create(0)
{

    m_ui = new Ui::KCMWebAccounts();
    m_ui->setupUi(this);

    m_ui->accList->setIconSize(QSize(48, 48));

    connect(m_ui->addBtn, SIGNAL(clicked(bool)), this, SLOT(addBtnClicked()));
    connect(m_ui->accList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(currentItemChanged(QListWidgetItem*,QListWidgetItem*)));

    QMetaObject::invokeMethod(this, "addExistingAccounts", Qt::QueuedConnection);
}

WebAccounts::~WebAccounts()
{
    delete m_ui;
}

void WebAccounts::addExistingAccounts()
{
    KSharedConfigPtr config = KSharedConfig::openConfig("webaccounts");
    QStringList groups = config->groupList();

    Q_FOREACH(const QString &group, groups) {
        QStringList accounts = config->group(group).groupList();
        Q_FOREACH(const QString &account, accounts) {
            addAccount(account, group);
        }
    }

    m_newAccountItem = new QListWidgetItem(i18n("New Account"), m_ui->accList);
    m_newAccountItem->setIcon(QIcon::fromTheme("applications-education-miscellaneous"));
    m_ui->accList->addItem(m_newAccountItem);

    if (m_ui->accList->count() == 0) {
        m_ui->accList->setCurrentItem(m_newAccountItem);
        return;
    }

    m_ui->accList->setCurrentItem(m_ui->accList->item(0));
}

void WebAccounts::addAccount(const QString& name, const QString& type)
{
    QListWidgetItem *newItem = new QListWidgetItem(name, m_ui->accList);
    newItem->setIcon(QIcon::fromTheme("gmail"));

    m_ui->accList->addItem(newItem);
}

void WebAccounts::addBtnClicked()
{
    m_ui->accList->setCurrentItem(m_newAccountItem, QItemSelectionModel::SelectCurrent);
}

void WebAccounts::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current == m_newAccountItem) {
        if (!m_create) {
            m_create = new Create(this);
            connect(m_create, SIGNAL(newAccount(QString,QString)), this, SLOT(newAccount(QString,QString)));
        }
        m_ui->accountInfo->setTitle(i18n("Select a supported Web Account"));

        m_ui->accountInfo->layout()->addWidget(m_create->widget());
    }
}

void WebAccounts::newAccount(const QString& type, const QString& name)
{
    QListWidgetItem *newItem = new QListWidgetItem();
    newItem->setIcon(QIcon::fromTheme("gmail"));
    newItem->setText(name);

    int row = m_ui->accList->row(m_newAccountItem);
    m_ui->accList->takeItem(row);
    m_ui->accList->insertItem(row, newItem);
    m_ui->accList->addItem(m_newAccountItem);
}


#include "webaccounts.moc"
