/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CREATE_ACCOUNT_JOB_H
#define CREATE_ACCOUNT_JOB_H

#include "kaccounts_export.h"

#include <kjob.h>

#include <QStringList>

namespace Accounts
{
    class Account;
    class Manager;
    class AccountService;
}
namespace SignOn
{
    class Error;
    class Identity;
    class SessionData;
    class IdentityInfo;
}

/**
 * @brief Create a new account for the specified provider
 */
class KACCOUNTS_EXPORT CreateAccountJob : public KJob
{
    Q_OBJECT
    Q_PROPERTY(QString providerName READ providerName WRITE setProviderName NOTIFY providerNameChanged)
public:
    explicit CreateAccountJob(QObject* parent = nullptr);
    explicit CreateAccountJob(const QString &providerName, QObject* parent = nullptr);

    QString providerName() const { return m_providerName; }
    void setProviderName(const QString &name);
    void start()  override;

private Q_SLOTS:
    void processSession();
    void sessionError(const SignOn::Error &signOnError);
    void sessionResponse(const SignOn::SessionData &data);
    void info(const SignOn::IdentityInfo &info);
    void pluginFinished(const QString &screenName, const QString &secret, const QVariantMap &map);
    void pluginError(const QString &error);

Q_SIGNALS:
    void providerNameChanged();

private:
    void loadPluginAndShowDialog(const QString &pluginName);

    QString m_providerName;
    QStringList m_disabledServices;
    Accounts::Manager *m_manager = nullptr;
    Accounts::Account *m_account = nullptr;
    Accounts::AccountService *m_accInfo = nullptr;
    SignOn::Identity *m_identity = nullptr;
    bool m_done = false;
};
#endif //CREATE_ACCOUNT_JOB_H
