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
#include "ownclouddialog.h"

#include <QtCore/QDebug>

#include <QtGui/QDialog>
#include <QtGui/QVBoxLayout>

#include <Accounts/Manager>
#include <Accounts/AccountService>

#include <SignOn/Identity>
#include <SignOn/AuthSession>

CreateAccount::CreateAccount(const QString &providerName, QObject* parent)
 : KJob(parent)
 , m_providerName(providerName)
 , m_manager(new Accounts::Manager(this))
 , m_account(0)
 , m_accInfo(0)
 , m_identity(0)
{

}

void CreateAccount::start()
{
    qDebug() << m_providerName;
    if (m_providerName == QLatin1String("owncloud")) {
        QMetaObject::invokeMethod(this, "processSessionOwncloud");
    } else  {
        QMetaObject::invokeMethod(this, "processSession");
    }
}

void CreateAccount::processSessionOwncloud()
{
    OwncloudDialog* dialog = new OwncloudDialog();


    dialog->exec();
}

void CreateAccount::processSession()
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
    m_accInfo = new Accounts::AccountService(m_account, service, this);

    QVariantMap data = m_accInfo->authData().parameters();
    data.insert("Embedded", false);

    SignOn::SessionData sessionData(data);

    SignOn::AuthSessionP session = m_identity->createSession(m_accInfo->authData().method());
    connect(session, SIGNAL(error(SignOn::Error)), SLOT(error(SignOn::Error)));
    connect(session, SIGNAL(response(SignOn::SessionData)), SLOT(response(SignOn::SessionData)));

    session->process(sessionData, m_accInfo->authData().mechanism());
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
    qDebug() << "\t:" << data.getProperty("ScreenName");

    QString displayName = data.getProperty("ScreenName").toString();
    if (displayName.isEmpty()) {
        displayName = data.UserName();
    }

    m_account->setDisplayName(displayName);


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

    if (m_account->displayName().isEmpty()) {
        m_account->setDisplayName(info.userName());
    }
    m_account->setCredentialsId(info.id());

    Accounts::AuthData authData = m_accInfo->authData();
    m_account->setValue("auth/mechanism", authData.mechanism());
    m_account->setValue("auth/method", authData.method());
    QString base("auth/");
    base.append(authData.method());
    base.append("/");
    base.append(authData.mechanism());
    base.append("/");

    QVariantMap data = authData.parameters();
    QMapIterator<QString, QVariant> i(data);
    while (i.hasNext()) {
        i.next();
        m_account->setValue(base + i.key(), i.value());
    }

    m_account->setEnabled(true);

    Accounts::ServiceList services = m_account->services();
    Q_FOREACH(const Accounts::Service &service, services) {
        m_account->selectService(service);
        m_account->setEnabled(true);
    }

    m_account->sync();

    emitResult();
}

void CreateAccount::error(const SignOn::Error& error)
{
    qDebug() << "Error:";
    qDebug() << "\t" << error.message();
}