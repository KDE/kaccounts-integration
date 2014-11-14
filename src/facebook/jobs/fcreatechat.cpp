/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#include "fcreatechat.h"

#include <TelepathyQt/Types>
#include <TelepathyQt/Account>
#include <TelepathyQt/PendingReady>
#include <TelepathyQt/PendingOperation>
#include <TelepathyQt/AccountFactory>
#include <TelepathyQt/AccountManager>
#include <TelepathyQt/PendingAccount>

#include <KTp/wallet-utils.h>

#include <QDebug>
#include <KWallet/Wallet>

using namespace KWallet;

FCreateChat::FCreateChat(KConfigGroup& group, QObject* parent)
 : KJob(parent)
 , m_config(group)
{

}

FCreateChat::~FCreateChat()
{

}

void FCreateChat::start()
{
    QMetaObject::invokeMethod(this, "createAccount", Qt::QueuedConnection);
}

void FCreateChat::createAccount()
{
    Tp::registerTypes();

    Tp::AccountFactoryPtr  accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                       << Tp::Account::FeatureAvatar
                                                                       << Tp::Account::FeatureProtocolInfo
                                                                       << Tp::Account::FeatureProfile);

    m_manager = Tp::AccountManager::create(accountFactory);

    connect(m_manager->becomeReady(),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onAccountManagerReady(Tp::PendingOperation*)));
}

void FCreateChat::onAccountManagerReady(Tp::PendingOperation* op)
{
    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), 0, Wallet::Synchronous);
    if (!wallet) {
        return;
    }
    wallet->setFolder("WebAccounts");

    QByteArray username;
    wallet->readEntry("facebookUsername", username);
    username.append("@chat.facebook.com");

    QString plugin("gabble");
    QString protocol("jabber");
    QString displayName(username);

    QVariantMap values, properties;

    values["resource"] = QVariant::fromValue<QString>("WebAccounts");
    values["account"] = QVariant::fromValue<QString>(QString::fromAscii(username));
    values["server"] = QVariant::fromValue<QString>("chat.facebook.com");

    properties["org.freedesktop.Telepathy.Account.Enabled"] = QVariant::fromValue<bool>(true);

    Tp::PendingAccount *pa = m_manager->createAccount(plugin, protocol, displayName, values, properties);

    connect(pa, SIGNAL(finished(Tp::PendingOperation*)), SLOT(onAccountCreated(Tp::PendingOperation*)));
}

void FCreateChat::onAccountCreated(Tp::PendingOperation* op)
{
    Tp::PendingAccount *pendingAccount = qobject_cast<Tp::PendingAccount*>(op);

    if (pendingAccount->isError()) {
        qDebug() << "Can't create account";
        m_config.group("services").writeEntry("Chat", -1);
        setError(-1);
        emitResult();
        return;
    }

    Tp::AccountPtr account = pendingAccount->account();

    KConfigGroup privates(&m_config, "private");
    privates.writeEntry("chatPath", account->objectPath());
    privates.sync();

    m_config.group("services").writeEntry("Chat", 1);

    QString password;
    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), 0, Wallet::Synchronous);
    if (!wallet) {
        qWarning("Can't open wallet");
        m_config.group("services").writeEntry("Chat", -1);
        setError(-1);
        emitResult();
        return;
    }

    wallet->setFolder("WebAccounts");
    if (wallet->readPassword(m_config.name(), password) != 0) {
        qWarning("Can't open wallet");
        m_config.group("services").writeEntry("Chat", -1);
        setError(-1);
        emitResult();
        return;
    }

    KTp::WalletUtils::setAccountPassword(account, password);

    account->setRequestedPresence(Tp::Presence::available(QLatin1String("Online")));

    account->setIconName("im-facebook");
    account->setServiceName("facebook");

    emitResult();
}
