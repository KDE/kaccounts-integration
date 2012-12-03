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

#ifndef ACCOUNTS_MODEL_H
#define ACCOUNTS_MODEL_H

#include <QtCore/QAbstractListModel>

#include <Accounts/Account>

class AccountsModelPrivate;
class AccountsModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        explicit AccountsModel(QObject* parent = 0);
        virtual ~AccountsModel();

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    public Q_SLOTS:
        void accountCreated(Accounts::AccountId accountId);
        void accountRemoved(Accounts::AccountId accountId);
        void accountUpdated();

    private:
        AccountsModelPrivate *d;
};

#endif //ACCOUNTS_MODEL_H