/*
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *  SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

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
        DisplayNameRole = Qt::DisplayRole,
        IdRole = Qt::UserRole + 1,
        ServicesRole,
        EnabledRole,
        CredentialsIdRole,
        ProviderNameRole,
        IconNameRole,
        DataObjectRole,
    };
    explicit AccountsModel(QObject *parent = nullptr);
    ~AccountsModel() override;

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    class Private;
    Private *d;
};

#endif // ACCOUNTS_MODEL_H
