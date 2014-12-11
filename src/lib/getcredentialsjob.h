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

/**
 * A KJob for obtaining user's credentials for the given Accounts::AccountId
 */
class KACCOUNTS_EXPORT GetCredentialsJob : public KJob
{
    Q_OBJECT
public:
    /**
     * Constructs the job with auth method and mechanism coming from the service
     * type. If no service type is specified, the default will be used
     *
     * @param id AccountId for which the credentials will be obtained
     */
    explicit GetCredentialsJob(const Accounts::AccountId &id, QObject *parent = 0);
    /**
     * This version of the constructor allow passing specific auth method and mechanism
     * for which we want the credentials
     *
     * For example some account has OAuth token and username-password credentials,
     * by setting both method and mechanism to "password", only the password will be
     * retrieved. Otherwise it depends on the passed serviceType - if there's no serviceType
     * set, it will use the default service for the given AccountId and will obtain
     * the credentials needed for that service
     *
     * @param id AccountId for which the credentials will be obtained
     * @param authMethod Auth method for which the credentials will be obtained
     * @param authMechanism Auth mechanism for which the credentials will be obtained
     */
    GetCredentialsJob(const Accounts::AccountId &id, const QString &authMethod = QString(), const QString &authMechanism = QString(), QObject *parent = 0);

    /**
     * Starts the credentials job
     */
    virtual void start();

    /**
     * Set service for which the auth method and mechanism will be selected
     *
     * @param serviceType Account's service type
     */
    void setServiceType(const QString &serviceType);

    /**
     * The obtained credentials data
     *
     * This will be valid only after the job has finished
     *
     * @returns Map with the credentials
     */
    QVariantMap credentialsData() const;

    /**
     * @returns Account id for which the credentials are obtained
     */
    Accounts::AccountId accountId() const;

private Q_SLOTS:
    void getCredentials();

private:
    class Private;
    Private * const d;
};

#endif //GET_CREDENTIALS_JOB_H
