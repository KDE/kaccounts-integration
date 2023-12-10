/*
 *  SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef PROVIDERS_MODEL_H
#define PROVIDERS_MODEL_H

#include "kaccounts_export.h"

#include <QAbstractListModel>

namespace KAccounts
{

/**
 * @brief A model which represents the available providers
 *
 * # Roles
 *
 * The role names available in this model are:
 *
 * * name: The internal name identifying this provider
 * * displayName: The human-readable name for this provider
 * * description: A (usually single sentence) description of this provider
 * * iconName: An XDG Icon specification icon name for this provider
 * * supportsMultipleAccounts: Whether or not this provider supports multiple simultaneous accounts
 * * accountsCount: The number of accounts which already exist on the system for this provider
 */
class KACCOUNTS_EXPORT ProvidersModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole + 1, ///< The unique name identifier for this provider
        DisplayNameRole, ///< The human-readable name for this provider
        DescriptionRole, ///< A (usually single sentence) description of this provider
        IconNameRole, ///< The name of the icon to be used for this provider (an XDG Icon Spec style name)
        SupportsMultipleAccountsRole, ///< Whether or not this provider supports multiple simultaneous accounts
        AccountsCountRole, ///< The number of accounts which already exist for this provider
    };
    explicit ProvidersModel(QObject *parent = nullptr);
    ~ProvidersModel() override;

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    class Private;
    Private *d;
};

};

#endif // PROVIDERS_MODEL_H
