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

#ifndef SERVICES_MODEL_H
#define SERVICES_MODEL_H

#include "kaccounts_export.h"

#include <QAbstractListModel>

/**
 * @brief A model which represents the services in a single account
 *
 * You can create this manually, but usually you would get an instance of it from the
 * AccountsModel::Roles::ServicesRole data role (model.services) of an AccountsModel
 * instance.
 */
class KACCOUNTS_EXPORT ServicesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* account READ account WRITE setAccount NOTIFY accountChanged)
    Q_PROPERTY(quint32 accountId READ accountId NOTIFY accountChanged)
    Q_PROPERTY(QString accountDisplayName READ accountDisplayName NOTIFY accountChanged)
    Q_PROPERTY(QString accountProviderName READ accountProviderName NOTIFY accountChanged)
    Q_PROPERTY(QString accountIconName READ accountIconName NOTIFY accountChanged)
    public:
        enum Roles {
            NameRole = Qt::UserRole + 1,
            DescriptionRole,
            DisplayNameRole,
            ServiceTypeRole,
            ProviderNameRole,
            IconNameRole,
            TagsRole,
            EnabledRole
        };
        explicit ServicesModel(QObject* parent = nullptr);
        virtual ~ServicesModel();

        QHash< int, QByteArray > roleNames() const override;
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        void setAccount(QObject* account);
        QObject* account() const;
        quint32 accountId() const;
        QString accountDisplayName() const;
        QString accountProviderName() const;
        QString accountIconName() const;
        Q_SIGNAL void accountChanged();

    private:
        class Private;
        Private *d;
};

#endif //ACCOUNTS_MODEL_H
