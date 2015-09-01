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

#ifndef CREATE_ACCOUNT_JOB_H
#define CREATE_ACCOUNT_JOB_H

#include <kjob.h>

namespace Accounts
{
    class Account;
    class Manager;
    class AccountService;
};
namespace SignOn
{
    class Error;
    class Identity;
    class SessionData;
    class IdentityInfo;
};

class CreateAccount : public KJob
{
    Q_OBJECT
    Q_PROPERTY(QString providerName READ providerName WRITE setProviderName NOTIFY providerNameChanged)
public:
    explicit CreateAccount(QObject* parent = 0);
    explicit CreateAccount(const QString &providerName, QObject* parent = 0);

    QString providerName() const { return m_providerName; }
    void setProviderName(const QString &name);
    void start() Q_DECL_OVERRIDE;

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
    Accounts::Manager *m_manager;
    Accounts::Account *m_account;
    Accounts::AccountService *m_accInfo;
    SignOn::Identity *m_identity;
    bool m_done;
};
#endif //CREATE_ACCOUNT_JOB_H
