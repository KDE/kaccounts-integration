/*
 *  SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "providersmodel.h"

#include "core.h"

#include <Accounts/Manager>
#include <Accounts/Provider>

class ProvidersModel::Private
{
public:
    Private()
    {
    }
    Accounts::Manager *accountsManager{nullptr};

    const Accounts::ProviderList &providerList()
    {
        if (!accountsManager) {
            accountsManager = KAccounts::accountsManager();
            providers = accountsManager->providerList();
        }
        return providers;
    }

private:
    Accounts::ProviderList providers;
};

ProvidersModel::ProvidersModel(QObject *parent)
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
    static const QHash<int, QByteArray> roleNames{
        {NameRole, "name"},
        {DisplayNameRole, "displayName"},
        {DescriptionRole, "description"},
        {IconNameRole, "iconName"},
        {SupportsMultipleAccountsRole, "supportsMultipleAccounts"},
        {AccountsCountRole, "accountsCount"},
    };
    return roleNames;
}

int ProvidersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return d->providerList().count();
}

QVariant ProvidersModel::data(const QModelIndex &index, int role) const
{
    QVariant data;
    if (checkIndex(index)) {
        const Accounts::Provider &provider = d->providerList().value(index.row());
        if (provider.isValid()) {
            switch (role) {
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
            case AccountsCountRole: {
                const Accounts::AccountIdList accounts = d->accountsManager->accountList();
                int i{0};
                for (const Accounts::AccountId &accountId : accounts) {
                    Accounts::Account *account = d->accountsManager->account(accountId);
                    if (account->providerName() == provider.name()) {
                        ++i;
                    }
                }
                data.setValue<int>(i);
                break;
            }
            default:
                data.setValue(QStringLiteral("No such role: %1").arg(role));
                break;
            }
        }
    }
    return data;
}
