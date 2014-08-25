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
    } else if (m_providerName.startsWith(QLatin1String("ktp"))) {
        QMetaObject::invokeMethod(this, "processSessionKTp");
    } else {
        QMetaObject::invokeMethod(this, "processSession");
    }
}

void CreateAccount::processSessionKTp()
{
    QString pluginPath;

    QStringList paths = QCoreApplication::libraryPaths();
    Q_FOREACH (const QString &libraryPath, paths) {
        QString path(libraryPath + QStringLiteral("/kaccounts/ui"));
        QDir dir(path);

        if (!dir.exists()) {
            continue;
        }

        QStringList entryList = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        Q_FOREACH (const QString &fileName, entryList) {
            if (fileName == QLatin1String("ktpaccountskcm_plugin_kaccounts.so")) {
                pluginPath = dir.absoluteFilePath(fileName);
            }
        }
    }

    QPluginLoader loader(pluginPath);

    if (!loader.load()) {
        qWarning() << "Could not create Extractor: " << pluginPath;
        qWarning() << loader.errorString();
        return;
    }

    QObject *obj = loader.instance();
    if (obj) {
        KAccountsUiPlugin *ui = qobject_cast<KAccountsUiPlugin*>(obj);
        if (!ui) {
            qDebug() << "Plugin could not be converted to an ExtractorPlugin";
            qDebug() << pluginPath;
            return;
        }

        connect(ui, SIGNAL(success(QString,QString,QVariantMap)),
                this, SLOT(ktpDialogFinished(QString,QString,QVariantMap)));

        connect(ui, SIGNAL(error(QString)),
                this, SLOT(ktpDialogError(QString)));

        // When the plugin has finished building the UI, show it right away
        connect(ui, &KAccountsUiPlugin::uiReady, [=](){ui->showDialog();});

        // Pass the provider name without the "ktp-" prefix, the rest matches
        // a Telepathy service name, which allows to open a specialized KTp
        // dialog for creating that particular account
        ui->setProviderName(m_providerName.mid(4));
    } else {
        qDebug() << "Plugin could not creaate instance" << pluginPath;
    }
}

void CreateAccount::ktpDialogFinished(const QString &username, const QString &password, const QVariantMap &additionalData)
{
    // Set up the new identity
    SignOn::IdentityInfo info;
    info.setUserName(username);
    info.setSecret(password);
    info.setCaption(username);
    info.setAccessControlList(QStringList(QLatin1String("*")));
    info.setType(SignOn::IdentityInfo::Application);

    m_identity = SignOn::Identity::newIdentity(info, this);
    m_identity->storeCredentials();

    m_account = m_manager->createAccount(m_providerName);
    // The "uid" in additionalData is Telepathy's Account object path
    // we need this to have a KTp::Account<-->KAccounts::Account mapping
    m_account->setValue("uid", additionalData.value("uid").toString());
    m_done = true;

    Accounts::Service service;
    m_accInfo = new Accounts::AccountService(m_account, service, this);
    connect(m_identity, SIGNAL(info(SignOn::IdentityInfo)), SLOT(info(SignOn::IdentityInfo)));
    // Proceed to finish creating the new Account
    m_identity->queryInfo();
}

void CreateAccount::ktpDialogError(const QString &error)
{
    qWarning() << "Error while creating KTp account:" << error;
}

void CreateAccount::processSessionOwncloud()
{
    OwncloudDialog* dialog = new OwncloudDialog();
    int result = dialog->exec();
    if (result == QDialog::Rejected) {
        setError(-1);
        setErrorText("Dialog was cancel");
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

    m_account = m_manager->createAccount(m_providerName);
    m_account->setValue("dav/host", dialog->host());

    Accounts::Service service;
    m_accInfo = new Accounts::AccountService(m_account, service, this);

    connect(m_identity, SIGNAL(info(SignOn::IdentityInfo)), SLOT(info(SignOn::IdentityInfo)));
    m_identity->queryInfo();
}

void CreateAccount::accountCreated()
{
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

    if (m_providerName.startsWith(QLatin1String("ktp-"))) {
        QString uid = m_account->value("uid").toString();

        KSharedConfigPtr kaccountsConfig = KSharedConfig::openConfig(QStringLiteral("kaccounts-ktprc"));
        KConfigGroup ktpKaccountsGroup = kaccountsConfig->group(QStringLiteral("ktp-kaccounts"));
        ktpKaccountsGroup.writeEntry(uid, m_account->id());

        KConfigGroup kaccountsKtpGroup = kaccountsConfig->group(QStringLiteral("kaccounts-ktp"));
        kaccountsKtpGroup.writeEntry(QString::number(m_account->id()), uid);
    }
}

void CreateAccount::error(const SignOn::Error& error)
{
    qWarning() << "Error:";
    qWarning() << "\t" << error.message();
}