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

#ifndef PROVIDERS_MODEL_H
#define PROVIDERS_MODEL_H

#include "kaccounts_export.h"

#include <QAbstractListModel>

/**
 * @brief A model which represents the available providers
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
            SupportsMultipleLoginsRole, ///< Whether or not this provider supports multiple simultaneous accounts
            AccountsCountRole ///< The number of accounts which already exist for this provider
        };
        explicit ProvidersModel(QObject* parent = nullptr);
        virtual ~ProvidersModel();

        QHash< int, QByteArray > roleNames() const override;
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        class Private;
        Private *d;
};

#endif//PROVIDERS_MODEL_H
