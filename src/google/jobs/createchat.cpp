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

#include "createchat.h"

#include <TelepathyQt/Types>
#include <TelepathyQt/Account>
#include <TelepathyQt/PendingReady>
#include <TelepathyQt/PendingOperation>
#include <TelepathyQt/AccountFactory>
#include <TelepathyQt/PendingAccount>

#include <KTp/wallet-interface.h>

#include <KDebug>
#include <KWallet/Wallet>

using namespace KWallet;

CreateChat::CreateChat(KConfigGroup group, QObject* parent)
: KJob(parent)
, m_config(group)
{

}

CreateChat::~CreateChat()
{
    m_manager->deleteLater();
}

void CreateChat::start()
{
    QMetaObject::invokeMethod(this, "createAccount", Qt::QueuedConnection);
}

void CreateChat::createAccount()
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

void CreateChat::onAccountManagerReady(Tp::PendingOperation* op)
{
    if (op->isError()) {
        kDebug() << "Can't create account";
        m_config.group("services").writeEntry("Chat", -1);
        setError(-1);
        emitResult();
        return;
    }
    QString plugin("gabble");
    QString protocol("jabber");
    QString displayName(m_config.name());

    QVariantMap values, properties;
    values["resource"] = QVariant::fromValue<QString>("WebAccounts");
    values["account"] = QVariant::fromValue<QString>(m_config.name());
    values["server"] = QVariant::fromValue<QString>("talk.google.com");
    values["fallback-conference-server"] = QVariant::fromValue<QString>("conference.jabber.org");
    values["old-ssl"] = QVariant::fromValue<bool>(true);
    values["port"] = QVariant::fromValue<uint>(5223);

    properties["org.freedesktop.Telepathy.Account.Enabled"] = QVariant::fromValue<bool>(true);

    Tp::PendingAccount *pa = m_manager->createAccount(plugin, protocol, displayName, values, properties);

    connect(pa, SIGNAL(finished(Tp::PendingOperation*)), SLOT(onAccountCreated(Tp::PendingOperation*)));
}

void CreateChat::onAccountCreated(Tp::PendingOperation* op)
{
    Tp::PendingAccount *pendingAccount = qobject_cast<Tp::PendingAccount*>(op);

    if (pendingAccount->isError()) {
        kDebug() << "Can't create account";
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
    m_config.sync();

    QString password;
    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), 0, Wallet::Synchronous);
    if (!wallet) {
        qWarning("Can't open wallet");
        setError(-1);
        wallet->deleteLater();
        emitResult();
        return;
    }

    wallet->setFolder("WebAccounts");
    if (wallet->readPassword("google-" + m_config.name(), password) != 0) {
        qWarning("Can't open wallet");
        setError(-1);
        wallet->deleteLater();
        emitResult();
        return;
    }
    wallet->deleteLater();

    KTp::WalletInterface::setPassword(account, password);

    account->setRequestedPresence(Tp::Presence::available(QLatin1String("Online")));

    account->setIconName("im-google-talk");

    emitResult();
}
