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

#ifndef ACCOUNTS_MODEL_H
#define ACCOUNTS_MODEL_H

#include "kaccounts_export.h"

#include <QAbstractListModel>

#include <Accounts/Account>

/**
 * @brief A model representing all the accounts registered on a system
 *
 * # Roles
 *
 * The following role names are available in this model:
 *
 * * id: The internal ID of the account
 * * services: A model which contains information about the services this account supports (see ServicesModel)
 * * enabled: Whether or not this account is enabled
 * * credentialsId: The internal ID for any stored credentials for this account
 * * displayName: A human-readable name for this account (change this using ChangeAccountDisplayNameJob)
 * * providerName: The internal name of the provider this account is registered through
 * * iconName: An XDG Icon specification icon name
 * * dataObject: The instance of Accounts::Account which the data for this account is fetched from
 */
class KACCOUNTS_EXPORT AccountsModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        enum Roles {
            IdRole = Qt::UserRole + 1,
            ServicesRole,
            EnabledRole,
            CredentialsIdRole,
            DisplayNameRole,
            ProviderNameRole,
            IconNameRole,
            DataObjectRole
        };
        explicit AccountsModel(QObject* parent = nullptr);
        virtual ~AccountsModel();

        QHash< int, QByteArray > roleNames() const override;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        class Private;
        Private *d;
};

#endif //ACCOUNTS_MODEL_H
