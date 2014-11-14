/*************************************************************************************
 *  Copyright (C) 2013 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
 *                                                                                   *
 *  This library is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU Lesser General Public                       *
 *  License as published by the Free Software Foundation; either                     *
 *  version 2 of the License, or (at your option) any later version.                 *
 *                                                                                   *
 *  This library is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 *  Library General Public License for more details.                                 *
 *                                                                                   *
 *  You should have received a copy of the GNU Library General Public License        *
 *  along with this library; see the file COPYING.LIB.  If not, write to             *
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,             *
 *  Boston, MA 02110-1301, USA.                                                      *
 *************************************************************************************/

#ifndef GET_CREDENTIALS_JOB_H
#define GET_CREDENTIALS_JOB_H

#include "kaccounts_export.h"

#include <KJob>
#include <Accounts/Account>

namespace Accounts {
    class Manager;
}

class KACCOUNTS_EXPORT GetCredentialsJob : public KJob
{
    Q_OBJECT
public:
    GetCredentialsJob(const Accounts::AccountId &id, QObject *parent = 0);
    GetCredentialsJob(const Accounts::AccountId &id, const QString &authMethod = QString(), const QString &authMechanism = QString(), QObject *parent = 0);
    virtual void start();

    void setServiceType(const QString &serviceType);

    QVariantMap credentialsData() const;
    Accounts::AccountId accountId() const;

private Q_SLOTS:
    void getCredentials();

private:
    class Private;
    Private * const d;
};

#endif //GET_CREDENTIALS_JOB_H
