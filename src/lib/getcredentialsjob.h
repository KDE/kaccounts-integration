/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef GET_CREDENTIALS_JOB_H
#define GET_CREDENTIALS_JOB_H

#include "kaccounts_export.h"

#include <KJob>
#include <Accounts/Account>

namespace Accounts {
    class Manager;
}

/**
 * @brief A KJob for obtaining user's credentials for the given Accounts::AccountId
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
    explicit GetCredentialsJob(Accounts::AccountId id, QObject *parent = nullptr);
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
    GetCredentialsJob(Accounts::AccountId id, const QString &authMethod = QString(), const QString &authMechanism = QString(), QObject *parent = nullptr);

    ~GetCredentialsJob();
    /**
     * Starts the credentials job
     */
    void start() override;

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

private:
    class Private;
    Private * const d;
    Q_PRIVATE_SLOT(d, void getCredentials())
};

#endif //GET_CREDENTIALS_JOB_H
