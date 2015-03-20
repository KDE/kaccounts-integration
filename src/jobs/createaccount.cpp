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
#include "lib/kaccountsuiplugin.h"

#include <QDebug>

#include <QPluginLoader>
#include <QDir>
#include <QDialog>
#include <QVBoxLayout>

#include <Accounts/Manager>
#include <Accounts/AccountService>

#include <SignOn/Identity>
#include <SignOn/AuthSession>

#include <KSharedConfig>
#include <KConfigGroup>

CreateAccount::CreateAccount(const QString &providerName, QObject* parent)
 : KJob(parent)
 , m_providerName(providerName)
 , m_manager(new Accounts::Manager(this))
 , m_account(0)
 , m_accInfo(0)
 , m_identity(0)
 , m_done(false)
{

}

void CreateAccount::start()
{
    qDebug() << m_providerName;
    if (m_providerName == QLatin1String("owncloud")) {
        QMetaObject::invokeMethod(this, "processSessionOwncloud");
    } else {
        QMetaObject::invokeMethod(this, "processSession");
    }
}

void CreateAccount::processSessionOwncloud()
{
    OwncloudDialog* dialog = new OwncloudDialog();
    int result = dialog->exec();
    if (result == QDialog::Rejected) {
        setError(-1);
        // this should not be user visible string
        setErrorText("Dialog was canceled");
        emitResult();
        return;
    }

    SignOn::IdentityInfo info;
    info.setUserName(dialog->username());
    info.setSecret(dialog->password());
    info.setCaption(m_providerName);
    info.setAccessControlList(QStringList(QLatin1String("*")));
    info.setType(SignOn::IdentityInfo::Application);

    m_identity = SignOn::Identity::newIdentity(info, this);
    m_identity->storeCredentials();
    m_done = true;

    m_account = m_manager->createAccount(m_providerName);
    m_account->setValue("dav/host", dialog->host());

    Accounts::Service service;
    m_accInfo = new Accounts::AccountService(m_account, service, this);

    connect(m_identity, SIGNAL(info(SignOn::IdentityInfo)), SLOT(info(SignOn::IdentityInfo)));
    m_identity->queryInfo();
}

void CreateAccount::accountCreated()
{
    if (m_providerName.startsWith(QLatin1String("ktp-"))) {
        QString uid = m_account->value("uid").toString();

        KSharedConfigPtr kaccountsConfig = KSharedConfig::openConfig(QStringLiteral("kaccounts-ktprc"));
        KConfigGroup ktpKaccountsGroup = kaccountsConfig->group(QStringLiteral("ktp-kaccounts"));
        ktpKaccountsGroup.writeEntry(uid, m_account->id());

        KConfigGroup kaccountsKtpGroup = kaccountsConfig->group(QStringLiteral("kaccounts-ktp"));
        kaccountsKtpGroup.writeEntry(QString::number(m_account->id()), uid);
        qDebug() << "Syncing config";
        kaccountsConfig->sync();
    }

    emitResult();
}

void CreateAccount::processSession()
{
    m_account = m_manager->createAccount(m_providerName);

    SignOn::IdentityInfo info;
    info.setCaption(m_providerName);
    info.setAccessControlList(QStringList("*"));
    info.setType(SignOn::IdentityInfo::Application);
    info.setStoreSecret(true);

    m_identity = SignOn::Identity::newIdentity(info, this);
    m_identity->storeCredentials();

    connect(m_identity, SIGNAL(info(SignOn::IdentityInfo)), SLOT(info(SignOn::IdentityInfo)));

    Accounts::Service service;
    m_accInfo = new Accounts::AccountService(m_account, service, this);

    QVariantMap data = m_accInfo->authData().parameters();
    data.insert("Embedded", false);

    SignOn::SessionData sessionData(data);

    SignOn::AuthSessionP session = m_identity->createSession(m_accInfo->authData().method());
    connect(session, SIGNAL(error(SignOn::Error)), SLOT(sessionError(SignOn::Error)));
    connect(session, SIGNAL(response(SignOn::SessionData)), SLOT(sessionResponse(SignOn::SessionData)));

    session->process(sessionData, m_accInfo->authData().mechanism());
}

void CreateAccount::sessionResponse(const SignOn::SessionData& data)
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

    m_done = true;
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

    if (!m_done) {
        return;
    }

    if (m_account->displayName().isEmpty()) {
        m_account->setDisplayName(info.userName());
    }
    m_account->setValue("username", info.userName());

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

    connect(m_account, SIGNAL(synced()), SLOT(accountCreated()));
    m_account->sync();
}

void CreateAccount::sessionError(const SignOn::Error &signOnError)
{
    if (error()) {
        // Guard against SignOn sending two error() signals
        return;
    }
    qWarning() << "Error:";
    qWarning() << "\t" << signOnError.message();

    setError(KJob::UserDefinedError);
    setErrorText(i18n("There was an error while trying to process the request: %1", signOnError.message()));
    emitResult();
}
