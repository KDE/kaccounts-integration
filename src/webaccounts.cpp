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
#include "accountwidget.h"
#include "ui_kcm.h"
#include "jobs/createcontact.h"
#include "jobs/createcalendar.h"
#include "jobs/createtask.h"
#include "jobs/createmail.h"
#include "jobs/createchat.h"
#include "jobs/removechat.h"
#include "jobs/removeemail.h"
#include "jobs/removecalendar.h"
#include "jobs/removetask.h"
#include "jobs/removeakonadiresource.h"

#include <QDebug>

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QStackedLayout>

#include <kpluginfactory.h>

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
    m_ui->accList->setIconSize(QSize(64, 64));

    connect(m_ui->remoteBtn, SIGNAL(clicked(bool)), this, SLOT(rmBtnClicked()));
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
    QStringList accountsList = accounts().groupList();
    qDebug() << "Existing accounts: " << accountsList;
    Q_FOREACH(const QString &account, accountsList) {
        addAccount(account, account);
    }

    m_create = new Create(this);
    connect(m_create, SIGNAL(newAccount(QString,QString)), this, SLOT(newAccount(QString,QString)));

    QWidget *widget = m_create->widget();
    m_newAccountItem = createQListWidgetItem(i18n("New Account"), "applications-education-miscellaneous", i18n("Select a supported Web Account"), widget);

    m_layout->addWidget(widget);

    m_ui->accList->addItem(m_newAccountItem);

    if (m_ui->accList->count() == 0) {
        m_ui->accList->setCurrentItem(m_newAccountItem);
        return;
    }

    m_ui->accList->setCurrentItem(m_ui->accList->item(0));
}

void WebAccounts::addAccount(const QString& name, const QString& accountName)
{
    AccountWidget *accountWidget = new AccountWidget(accountName, this);
    m_layout->addWidget(accountWidget);

    QListWidgetItem *newItem = createQListWidgetItem(name, "gmail", name, accountWidget);
    m_ui->accList->addItem(newItem);
}

void WebAccounts::addBtnClicked()
{
    m_ui->accList->setCurrentItem(m_newAccountItem, QItemSelectionModel::SelectCurrent);
}

void WebAccounts::rmBtnClicked()
{
    QListWidgetItem *item = m_ui->accList->currentItem();
    if (item == m_newAccountItem) {
        return;
    }

    QString accName = item->data(Qt::UserRole).toString();

    KConfigGroup group = account(accName);
    KConfigGroup services = group.group("services");

    if (services.readEntry("EMail", 0) == 1) {
        RemoveEmail *removeEmail = new RemoveEmail(group, this);
        connect(removeEmail, SIGNAL(finished(KJob*)), this, SLOT(serviceRemoved(KJob*)));
        removeEmail->start();
    }

    if (services.readEntry("Contact", 0) == 1) {
        RemoveAkonadiResource *removeContact = new RemoveAkonadiResource("contactResource", group, this);
        connect(removeContact, SIGNAL(finished(KJob*)), this, SLOT(serviceRemoved(KJob*)));
        removeContact->start();
    }

    if (services.readEntry("Calendar", 0) == 1) {
        RemoveCalendar *removeCalendar = new RemoveCalendar(group, this);
        connect(removeCalendar, SIGNAL(finished(KJob*)), this, SLOT(serviceRemoved(KJob*)));
        removeCalendar->start();
    }

     if (services.readEntry("Tasks", 0) == 1) {
        RemoveTask *removeCalendar = new RemoveTask(group, this);
        connect(removeCalendar, SIGNAL(finished(KJob*)), this, SLOT(serviceRemoved(KJob*)));
        removeCalendar->start();
     }

    if (services.readEntry("Chat", 0) == 1) {
        RemoveChat *removeChat = new RemoveChat(group, this);
        connect(removeChat, SIGNAL(finished(KJob*)), this, SLOT(serviceRemoved(KJob*)));
        removeChat->start();
    }

    m_ui->accList->takeItem(m_ui->accList->row(item));

    delete item->data(Qt::UserRole + 1).value<QWidget *>();
}

void WebAccounts::serviceRemoved(KJob *job)
{
    QString accName = job->objectName();
    KConfigGroup services = account(accName).group("services");
    QStringList keys = services.keyList();
    bool deleteAccount = true;
    Q_FOREACH(const QString &key, keys) {
        if (services.readEntry(key, 0) != 0) {
            deleteAccount = false;
            break;
        }
    }

    if (deleteAccount) {
        KSharedConfig::openConfig("webaccounts")->group("accounts").deleteGroup(accName);
    }
}

void WebAccounts::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current) {
        return;
    }

    m_ui->accountInfo->setTitle(current->data(Qt::UserRole).toString());
    m_layout->setCurrentWidget(current->data(Qt::UserRole + 1).value<QWidget *>());

    m_ui->remoteBtn->setEnabled(current != m_newAccountItem);
}

void WebAccounts::newAccount(const QString& type, const QString& name)
{
    AccountWidget *accountWidget = new AccountWidget(name, this);
    m_layout->addWidget(accountWidget);

    QListWidgetItem *newItem = createQListWidgetItem(name, "gmail", name, accountWidget);
    m_ui->accList->addItem(newItem);

    int row = m_ui->accList->row(m_newAccountItem);

    m_ui->accList->takeItem(row);
    m_ui->accList->insertItem(row, newItem);
    m_ui->accList->addItem(m_newAccountItem);

    m_ui->accList->setCurrentItem(newItem);

    KConfigGroup services = account(name).group("services");

#warning Fix Tasks but not Calendar
    if (services.readEntry("Contact", 0) == 2) {
        CreateContact *create = new CreateContact(account(name), this);
        create->start();
    }

    if (services.readEntry("Calendar", 0) == 2) {
        CreateCalendar *createCalendar = new CreateCalendar(account(name), this);
        connect(createCalendar, SIGNAL(result(KJob*)), this, SLOT(createTasks(KJob*)));
        createCalendar->start();
    }

    if (services.readEntry("EMail", 0) == 2) {
        CreateMail *createMail = new CreateMail(account(name), this);
        createMail->start();
    }

    if (services.readEntry("Chat", 0) == 2) {
        CreateChat *createChat = new CreateChat(account(name), this);
        createChat->start();
    }
}

void WebAccounts::createTasks(KJob* job)
{
    if (job->error()) {
        qWarning("Error creating calendar resource");
        return;
    }

    KConfigGroup group  = qobject_cast< CreateCalendar* >(job)->config();
    CreateTask *createTask = new CreateTask(group, this);
    createTask->start();
}

QListWidgetItem* WebAccounts::createQListWidgetItem(const QString& name, const QString& icon, const QString& title, QWidget *widget)
{
    QListWidgetItem *newItem = new QListWidgetItem();
    newItem->setIcon(QIcon::fromTheme(icon));
    newItem->setText(name);
    newItem->setData(Qt::UserRole, QVariant(title));
    newItem->setData(Qt::UserRole + 1, QVariant::fromValue<QWidget *>(widget));

    return newItem;
}

KConfigGroup WebAccounts::accounts()
{
    return KSharedConfig::openConfig("webaccounts")->group("accounts");
}

KConfigGroup WebAccounts::account(const QString& accName)
{
    return accounts().group(accName);
}

#include "webaccounts.moc"
