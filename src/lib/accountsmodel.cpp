/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
 *  Copyright (C) 2020 by Dan Leinir Turthra Jensen <admin@leinir.dk>                *
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

#include "accountsmodel.h"

#include "core.h"
#include "servicesmodel.h"

#include <QDebug>

#include <QIcon>
#include <klocalizedstring.h>

#include <SignOn/Identity>

#include <Accounts/Account>
#include <Accounts/Manager>

class AccountsModel::Private : public QObject
{
public:
    Private(AccountsModel *model)
        : accountsManager(KAccounts::accountsManager())
        , q(model)
    {
        accountIDs = accountsManager->accountList();

        connect(accountsManager, SIGNAL(accountCreated(Accounts::AccountId)), 
                q, SLOT(accountCreated(Accounts::AccountId)));
        connect(accountsManager, SIGNAL(accountRemoved(Accounts::AccountId)), 
                q, SLOT(accountRemoved(Accounts::AccountId)));
    };
    virtual ~Private()
    {
        qDeleteAll(accounts);
    };

    Accounts::Manager *accountsManager;
    Accounts::AccountIdList accountIDs;
    QHash<int, Accounts::Account*> accounts;
    QHash<Accounts::Account*, ServicesModel*> servicesModels;

    Accounts::Account* accountById(int id);
    void removeAccount(Accounts::AccountId accountId);

private:
    AccountsModel* q;
};

Accounts::Account* AccountsModel::Private::accountById(int id)
{
    if (accounts.contains(id)) {
        return accounts.value(id);
    }

    Accounts::Account* account = accountsManager->account(id);
    if (!account) {
        qDebug() << "\t Failed to get the account from manager";
        return nullptr;
    }

    connect(account, SIGNAL(displayNameChanged(QString)), q, SLOT(accountUpdated()));

    accounts[id] = account;
    return account;
}

void AccountsModel::Private::removeAccount(Accounts::AccountId accountId)
{
    accountIDs.removeOne(accountId);
    delete accounts.take(accountId);
}

AccountsModel::AccountsModel(QObject* parent)
 : QAbstractListModel(parent)
 , d(new AccountsModel::Private(this))
{
}

AccountsModel::~AccountsModel()
{
    delete d;
}

QHash<int, QByteArray> AccountsModel::roleNames() const
{
    static QHash<int, QByteArray> roles{
        {IdRole, "id"},
        {ServicesRole, "services"},
        {EnabledRole, "enabled"},
        {CredentialsIdRole, "credentialsId"},
        {DisplayNameRole, "displayName"},
        {ProviderNameRole, "providerName"},
        {IconNameRole, "iconName"},
        {DataObjectRole, "dataObject"}
    };
    return roles;
}

int AccountsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return d->accountIDs.count();
}

QVariant AccountsModel::data(const QModelIndex& index, int role) const
{
    QVariant data;
    if(checkIndex(index)) {
        Accounts::AccountId accountId = d->accountIDs.value(index.row());
        Accounts::Account *account = d->accountById(accountId);
        if (account) {
            switch (role) {
                case IdRole:
                    data.setValue(account->id());
                    break;
                case ServicesRole:
                {
                    ServicesModel* servicesModel{nullptr};
                    if (d->servicesModels.contains(account)) {
                        servicesModel = d->servicesModels.value(account);
                    } else {
                        // Not parenting to the account itself, so we can avoid it suddenly
                        // disappearing. Just to be on the safe side
                        servicesModel = new ServicesModel(d->accountsManager);
                        servicesModel->setAccount(account);
                        d->servicesModels[account] = servicesModel;
                    }
                    data.setValue(servicesModel);
                    break;
                }
                case EnabledRole:
                    data.setValue(account->enabled());
                    break;
                case CredentialsIdRole:
                    data.setValue(account->credentialsId());
                    break;
                case DisplayNameRole:
                    data.setValue(account->displayName());
                    break;
                case ProviderNameRole:
                    data.setValue(account->providerName());
                    break;
                case IconNameRole:
                {
                    QString iconName = account->provider().iconName();
                    if (iconName.isEmpty()) {
                        iconName = QString("unknown");
                    }
                    data.setValue(iconName);
                    break;
                }
                case DataObjectRole:
                    data.setValue<QObject*>(account);
                    break;
            }
        }
    }

    return data;
}

void AccountsModel::accountCreated(Accounts::AccountId accountId)
{
    qDebug() << "AccountsModel::accountCreated: " << accountId;
    int row = d->accountIDs.count();
    beginInsertRows(QModelIndex(), row, row);
    d->accountIDs.insert(row, accountId);
    endInsertRows();
}

void AccountsModel::accountRemoved(Accounts::AccountId accountId)
{
    qDebug() << "AccountsModel::accountRemoved: " << accountId;
    beginRemoveRows(QModelIndex(), d->accountIDs.indexOf(accountId), d->accountIDs.indexOf(accountId));
    d->removeAccount(accountId);
    endRemoveRows();
}

void AccountsModel::accountUpdated()
{
    Accounts::Account *acc = qobject_cast<Accounts::Account*>(sender());
    Accounts::AccountId accountId = acc->id();
    qDebug() << "Account updated: " << accountId;

    QModelIndex accountIndex = index(d->accountIDs.indexOf(accountId), 0);
    Q_EMIT dataChanged(accountIndex, accountIndex);
}
