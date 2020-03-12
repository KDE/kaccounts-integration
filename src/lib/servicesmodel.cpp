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

#include "servicesmodel.h"

#include "core.h"

#include <QDebug>

#include <QIcon>

#include <SignOn/Identity>

#include <Accounts/Account>
#include <Accounts/Manager>

class ServicesModel::Private : public QObject
{
public:
    Private(ServicesModel *model)
        : q(model)
    { };
    virtual ~Private()
    { };

    Accounts::ServiceList services;
    Accounts::Account* account{nullptr};

private:
    ServicesModel* q;
};

ServicesModel::ServicesModel(QObject* parent)
 : QAbstractListModel(parent)
 , d(new ServicesModel::Private(this))
{
}

ServicesModel::~ServicesModel()
{
    delete d;
}

QHash<int, QByteArray> ServicesModel::roleNames() const
{
    static QHash<int, QByteArray> roles{
        {NameRole, "name"},
        {DescriptionRole, "description"},
        {DisplayNameRole, "displayName"},
        {ServiceTypeRole, "servieType"},
        {ProviderNameRole, "providerName"},
        {IconNameRole, "iconName"},
        {TagsRole, "tags"},
        {EnabledRole, "enabled"}
    };
    return roles;
}

int ServicesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return d->services.count();
}

QVariant ServicesModel::data(const QModelIndex& index, int role) const
{
    QVariant data;
    if(checkIndex(index)) {
        const Accounts::Service& service = d->services.value(index.row());
        if (service.isValid()) {
            switch (role) {
                case NameRole:
                    data.setValue(service.name());
                    break;
                case DescriptionRole:
#if ACCOUNTSQT5_VERSION_MAJOR==1 && ACCOUNTSQT5_VERSION_MINOR>=16
                    // Not all services have descriptions and UIs should be designed with that in mind.
                    // Consequently, we can accept not having a fallback for this.
                    data.setValue(service.description());
#endif
                    break;
                case DisplayNameRole:
                    data.setValue(service.displayName());
                    break;
                case ServiceTypeRole:
                    data.setValue(service.serviceType());
                    break;
                case ProviderNameRole:
                    data.setValue(service.provider());
                    break;
                case IconNameRole:
                    data.setValue(service.iconName());
                    break;
                case TagsRole:
                    data.setValue(service.tags().values());
                    break;
                case EnabledRole:
                    data.setValue(d->account->enabledServices().contains(service));
                    break;
            }
        }
    }
    return data;
}

void ServicesModel::setAccount(QObject* account)
{
    if (d->account != account) {
        beginResetModel();
        d->services.clear();
        if (d->account) {
            disconnect(d->account, nullptr, this, nullptr);
        }
        d->account = qobject_cast<Accounts::Account*>(account);
        if (d->account) {
            connect(d->account, &Accounts::Account::displayNameChanged, this, &ServicesModel::accountChanged);
            connect(d->account, &Accounts::Account::enabledChanged, this, [this](const QString& serviceName, bool /*enabled*/){
                int i{0};
                for (const Accounts::Service& service : d->services) {
                    if (service.name() == serviceName) {
                        break;
                    }
                    ++i;
                }
                dataChanged(index(i), index(i));
            });
            connect(d->account, &QObject::destroyed, this, [this](){ 
                beginResetModel();
                d->account = nullptr;
                Q_EMIT accountChanged();
                d->services.clear();
                endResetModel();
            });
            d->services = d->account->services();
        }
        endResetModel();
        Q_EMIT accountChanged();
    }
}

QObject * ServicesModel::account() const
{
    return d->account;
}

quint32 ServicesModel::accountId() const
{
    if (d->account) {
        return d->account->id();
    }
    return -1;
}

QString ServicesModel::accountDisplayName() const
{
    if (d->account) {
        return d->account->displayName();
    }
    return QString{};
}

QString ServicesModel::accountProviderName() const
{
    if (d->account) {
        return d->account->providerName();
    }
    return QString{};
}

QString ServicesModel::accountIconName() const
{
    if (d->account && d->account->provider().isValid() && !d->account->provider().iconName().isEmpty()) {
        return d->account->provider().iconName();
    }
    return QString::fromLatin1("user-identity");
}
