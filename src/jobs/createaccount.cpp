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
#include "uipluginsmanager.h"

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

void CreateAccount::processSession()
{
    m_account = m_manager->createAccount(m_providerName);
    Accounts::Service service;
    m_accInfo = new Accounts::AccountService(m_account, service, this);

    const QString pluginName = m_account->provider().pluginName();
    if (!pluginName.isEmpty()) {
        loadPluginAndShowDialog(pluginName);
    } else {
        SignOn::IdentityInfo info;
        info.setCaption(m_providerName);
        info.setAccessControlList(QStringList("*"));
        info.setType(SignOn::IdentityInfo::Application);
        info.setStoreSecret(true);

        m_identity = SignOn::Identity::newIdentity(info, this);
        m_identity->storeCredentials();

        connect(m_identity, SIGNAL(info(SignOn::IdentityInfo)), SLOT(info(SignOn::IdentityInfo)));
        connect(m_identity, &SignOn::Identity::error, [=](const SignOn::Error &err) {
            qDebug() << "Error storing identity:" << err.message();
        });

        QVariantMap data = m_accInfo->authData().parameters();
        data.insert("Embedded", false);

        SignOn::SessionData sessionData(data);
        SignOn::AuthSessionP session = m_identity->createSession(m_accInfo->authData().method());
        qDebug() << "Starting auth session with" << m_accInfo->authData().method();
        connect(session, SIGNAL(error(SignOn::Error)), SLOT(sessionError(SignOn::Error)));
        connect(session, SIGNAL(response(SignOn::SessionData)), SLOT(sessionResponse(SignOn::SessionData)));

        session->process(sessionData, m_accInfo->authData().mechanism());
    }
}

void CreateAccount::loadPluginAndShowDialog(const QString &pluginName)
{
    KAccountsUiPlugin *ui = KAccounts::UiPluginsManager::pluginForName(pluginName);

    if (!ui) {
        qDebug() << "Plugin could not be loaded";
        pluginError(i18nc("The %1 is for plugin name, eg. Could not load UI plugin", "Could not load %1 plugin, please check your installation", pluginName));
        return;
    }

    connect(ui, SIGNAL(success(QString,QString,QVariantMap)),
            this, SLOT(pluginFinished(QString,QString,QVariantMap)));

    connect(ui, SIGNAL(error(QString)),
            this, SLOT(pluginError(QString)));

    ui->init(KAccountsUiPlugin::NewAccountDialog);

    // When the plugin has finished building the UI, show it right away
    connect(ui, &KAccountsUiPlugin::uiReady, ui, &KAccountsUiPlugin::showNewAccountDialog);

    ui->setProviderName(m_providerName);
}

void CreateAccount::pluginFinished(const QString &screenName, const QString &secret, const QVariantMap &data)
{
    // Set up the new identity
    SignOn::IdentityInfo info;
    info.setStoreSecret(true);
    info.setUserName(screenName);
    info.setSecret(secret, true);
    info.setCaption(m_providerName);
    info.setAccessControlList(QStringList(QLatin1String("*")));
    info.setType(SignOn::IdentityInfo::Application);

    m_identity = SignOn::Identity::newIdentity(info, this);
    connect(m_identity, SIGNAL(info(SignOn::IdentityInfo)), SLOT(info(SignOn::IdentityInfo)));



    m_done = true;

    m_identity->storeCredentials();
    connect(m_identity, &SignOn::Identity::credentialsStored, m_identity, &SignOn::Identity::queryInfo);

    // Delete the dialog
}

void CreateAccount::pluginError(const QString &error)
{
    if (error.isEmpty()) {
        setError(-1);
    } else {
        setError(KJob::UserDefinedError);
    }
    setErrorText(error);
    // Delete the dialog
    emitResult();
}

void CreateAccount::sessionResponse(const SignOn::SessionData &data)
{
    qDebug() << "Received session response";

    m_done = true;
    m_identity->queryInfo();
}

void CreateAccount::info(const SignOn::IdentityInfo &info)
{
    qDebug() << "Info:";
    qDebug() << "\tId:" << info.id();
    qDebug() << "\tcaption:" << info.caption();
    qDebug() << "\towner:" << info.owner();
    qDebug() << "\tuserName:" << info.userName();

    if (!m_done) {
        return;
    }

    if (m_account->displayName().isEmpty()) {
        m_account->setDisplayName(info.userName());
    }
    m_account->setValue("username", info.userName());

    m_account->selectService();
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
    connect(m_account, &Accounts::Account::synced, this, &CreateAccount::emitResult);
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
