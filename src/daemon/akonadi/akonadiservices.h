/*************************************************************************************
 *  Copyright (C) 2013 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#ifndef AKONADI_SERVICES_H
#define AKONADI_SERVICES_H

#include <QtCore/QMap>
#include <QtCore/QString>

#include <Accounts/Account>

class AkonadiAccounts;
class AkonadiServices : public QObject
{
    Q_OBJECT
    public:
        explicit AkonadiServices(QObject* parent = 0);

        void serviceAdded(const Accounts::AccountId& accId, QMap< QString, QString >& services);
        void serviceRemoved(const Accounts::AccountId& accId, QMap< QString, QString >& services);

    private:
        AkonadiAccounts* m_accounts;
};

#endif //AKONADI_SERVICES_H