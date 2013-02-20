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

#include "createaccount.h"

#include <QtCore/QDebug>

#include <Accounts/Manager>
#include <Accounts/AccountService>

#include <SignOn/Identity>
#include <SignOn/AuthSession>

CreateAccount::CreateAccount(const QString& providerName, QObject* parent)
 : KJob(parent)
 , m_providerName(providerName)
 , m_manager(new Accounts::Manager(this))
 , m_account(0)
 , m_identity(0)
{

}

void CreateAccount::start()
{
    m_account = m_manager->createAccount(m_providerName);

    SignOn::IdentityInfo info;
    info.setSecret("");
    info.setCaption(m_providerName);
    info.setAccessControlList(QStringList("*"));
    info.setType(SignOn::IdentityInfo::Application);

    m_identity = SignOn::Identity::newIdentity(info, this);
    m_identity->storeCredentials();

    Accounts::Service service;
    Accounts::AccountService* accInfo = new Accounts::AccountService(m_account, service);

    QVariantMap data = accInfo->authData().parameters();
    data.insert("Embedded", false);

    QStringList schemes;
    schemes.append("https");
    schemes.append("http");
    data.insert("AllowedSchemes", schemes);

    SignOn::SessionData sessionData(data);

    SignOn::AuthSessionP session = m_identity->createSession(accInfo->authData().method());
    qDebug() << connect(session, SIGNAL(error(SignOn::Error)), SLOT(error(SignOn::Error)));
    qDebug() << connect(session, SIGNAL(response(SignOn::SessionData)), SLOT(response(SignOn::SessionData)));

    session->process(sessionData, accInfo->authData().mechanism());
}

void CreateAccount::response(const SignOn::SessionData& data)
{
    qDebug() << "Response:";
    qDebug() << "\tToken:" << data.getProperty("AccessToken");
    qDebug() << "\tExpires:" << data.getProperty("ExpiresIn");
    qDebug() << "\tCaption:" << data.Caption();
    qDebug() << "\tControlTokens:" << data.getAccessControlTokens();
    qDebug() << "\tPropertyNames:" << data.propertyNames();
    qDebug() << "\tProxy:" << data.NetworkProxy();
    qDebug() << "\tRealm:" << data.Realm();
    qDebug() << "\tSecret:" << data.Secret();
    qDebug() << "\tUsername:" << data.UserName();

    connect(m_identity, SIGNAL(info(SignOn::IdentityInfo)), SLOT(info(SignOn::IdentityInfo)));
    m_identity->queryInfo();
}

void CreateAccount::info(const SignOn::IdentityInfo& info)
{
    qDebug() << "Info:";
    qDebug() << "\tId:" << info.id();
    qDebug() << "\taccessControl:" << info.accessControlList();
    qDebug() << "\tcaption:" << info.caption();
    qDebug() << "\tid:" << info.id();
    qDebug() << "\tstoringSecret:" << info.isStoringSecret();
    qDebug() << "\towner:" << info.owner();
    qDebug() << "\tuserName:" << info.userName();

    m_account->setDisplayName(info.userName());
    m_account->setCredentialsId(info.id());
//     account->setValue("auth/mechanism", "user_agent");
//     account->setValue("auth/method", "oauth2");
//     account->setValue("auth/oauth2/user_agent/AuthPath", "o/oauth2/auth");
//     account->setValue("auth/oauth2/user_agent/ClientId", "759250720802-4sii0me9963n9fdqdmi7cepn6ub8luoh.apps.googleusercontent.com");
//     account->setValue("auth/oauth2/user_agent/Host", "accounts.google.com");
//     account->setValue("auth/oauth2/user_agent/RedirectUri","https://wiki.ubuntu.com/");
//     account->setValue("auth/oauth2/user_agent/ResponseType", "token");
//     account->setValue("auth/oauth2/user_agent/Scope", scopes);
//     account->setValue("auth/oauth2/user_agent/TokenPath", "o/oauth2/token");
    m_account->setEnabled(true);
    m_account->sync();
}

void CreateAccount::error(const SignOn::Error& error)
{
    qDebug() << "Error:";
    qDebug() << "\t" << error.message();
}