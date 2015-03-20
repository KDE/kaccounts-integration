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

#include "kaccounts.h"
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
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDebug>

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(KAccountsFactory, "kcm_kaccounts.json", registerPlugin<KAccounts>();)

KAccounts::KAccounts(QWidget *parent, const QVariantList &)
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

    if (m_model->rowCount() == 0) {
        m_layout->setCurrentIndex(0);
    } else {
        QLabel *label = new QLabel(i18n("Select an account from the left column to configure"), this);
        label->setAlignment(Qt::AlignCenter);
        m_layout->addWidget(label);
        m_layout->setCurrentIndex(2);
    }

    m_ui->accountsView->setIconSize(QSize(32,32));
    m_ui->accountsView->setModel(m_model);
    m_ui->accountsView->setSelectionModel(m_selectionModel);

    connect(m_ui->removeBtn, SIGNAL(clicked(bool)), this, SLOT(rmBtnClicked()));
    connect(m_ui->addBtn, SIGNAL(clicked(bool)), this, SLOT(addBtnClicked()));

    QDBusMessage moduleLoadCall = QDBusMessage::createMethodCall(QStringLiteral("org.kde.kded5"), QStringLiteral("/kded"),
                                                                 QStringLiteral("org.kde.kded5"), QStringLiteral("loadModule"));
    moduleLoadCall.setArguments(QList<QVariant>{QVariant("accounts")});

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(moduleLoadCall);
    QDBusPendingCallWatcher *pendingCallWatcher = new QDBusPendingCallWatcher(pendingCall, this);
    connect(pendingCallWatcher, &QDBusPendingCallWatcher::finished,
            this, &KAccounts::moduleLoadCallFinished);
}

void KAccounts::currentChanged(const QModelIndex &current, const QModelIndex &previous)
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

KAccounts::~KAccounts()
{
    delete m_ui;
}

void KAccounts::addBtnClicked()
{
    m_selectionModel->setCurrentIndex(m_model->index(m_model->rowCount() - 1), QItemSelectionModel::SelectCurrent);
}

void KAccounts::rmBtnClicked()
{
    QModelIndex index = m_selectionModel->currentIndex();
    if (!index.isValid()) {
        return;
    }
    // TODO: ask confirmation first?
    m_model->removeRows(index.row(), 1);
}

void KAccounts::moduleLoadCallFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<bool> reply = *watcher;
    bool loaded;
    if (reply.isError()) {
        loaded = false;
    } else {
        loaded = reply.value();
    }

    if (!loaded) {
        qWarning() << "Unable to start the kded module, things may (and most probably will) misbehave";
    }
}

#include "kaccounts.moc"
