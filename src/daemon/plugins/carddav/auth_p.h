/*
 * Martin Klapetek (C) 2015 <mklapetek@kde.org>
 * This file is based on buteo-sync-plugin-carddav by Jolla Ltd.
 *
 * Copyright (C) 2014 Jolla Ltd. and/or its subsidiary(-ies).
 *
 * Contributors: Chris Adams <chris.adams@jolla.com>
 *
 * This program/library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program/library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program/library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <QObject>

#include <Accounts/Account>
#include <Accounts/Manager>
#include <Accounts/Service>
#include <Accounts/AccountService>
#include <SignOn/Identity>
#include <SignOn/Error>
#include <SignOn/SessionData>
#include <SignOn/AuthSession>

class Auth : public QObject
{
    Q_OBJECT

public:
    Auth(QObject *parent);
    ~Auth();

    void signIn(int accountId);
    void setCredentialsNeedUpdate(int accountId);

Q_SIGNALS:
    void signInCompleted(const QString &serverUrl, const QString &addressbookPath, const QString &username, const QString &password, const QString &accessToken, bool ignoreSslErrors);
    void signInError();

private Q_SLOTS:
    void signOnResponse(const SignOn::SessionData &response);
    void signOnError(const SignOn::Error &error);

private:
    Accounts::Manager m_manager;
    Accounts::Account *m_account;
    SignOn::Identity *m_ident;
    SignOn::AuthSession *m_session;
    QString m_serverUrl;
    QString m_addressbookPath;
    bool m_ignoreSslErrors;
};


