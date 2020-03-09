/*************************************************************************************
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

#include "providersmodel.h"

#include "core.h"

#include <Accounts/Provider>
#include <Accounts/Manager>

class ProvidersModel::Private {
public:
    Private() {}
    Accounts::Manager* accountsManager{nullptr};

    const Accounts::ProviderList& providerList() {
        if (!accountsManager) {
            accountsManager = KAccounts::accountsManager();
            providers = accountsManager->providerList();
        }
        return providers;
    }
private:
    Accounts::ProviderList providers;
};

ProvidersModel::ProvidersModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
}

ProvidersModel::~ProvidersModel()
{
    delete d;
}

QHash<int, QByteArray> ProvidersModel::roleNames() const
{
    static const QHash<int, QByteArray> roleNames {
        {NameRole, "name"},
        {DisplayNameRole, "displayName"},
        {DescriptionRole, "description"},
        {IconNameRole, "iconName"},
        {SupportsMultipleAccountsRole, "supportsMultipleAccounts"},
        {AccountsCountRole, "accountsCount"}
    };
    return roleNames;
}

int ProvidersModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return d->providerList().count();
}

QVariant ProvidersModel::data(const QModelIndex& index, int role) const
{
    QVariant data;
    if (checkIndex(index)) {
        const Accounts::Provider& provider = d->providerList().value(index.row());
        if (provider.isValid()) {
            switch(role) {
                case NameRole:
                    data.setValue(provider.name());
                    break;
                case DisplayNameRole:
                    data.setValue(provider.displayName());
                    break;
                case DescriptionRole:
                    data.setValue(provider.description());
                    break;
                case IconNameRole:
                    data.setValue(provider.iconName());
                    break;
                case SupportsMultipleAccountsRole:
                    data.setValue(!provider.isSingleAccount());
                    break;
                case AccountsCountRole:
                {
                    const Accounts::AccountIdList accounts = d->accountsManager->accountList();
                    int i{0};
                    for (const Accounts::AccountId& accountId : accounts) {
                        Accounts::Account* account = d->accountsManager->account(accountId);
                        if (account->providerName() == provider.name()) {
                            ++i;
                        }
                    }
                    data.setValue<int>(i);
                    break;
                }
                default:
                    data.setValue(QString::fromLatin1("No such role: %1").arg(role));
                    break;
            }
        }
    }
    return data;
}
