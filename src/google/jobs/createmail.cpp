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

#include "createmail.h"
#include "akonadi_imap_settings.h"

#include <QDBusInterface>

#include <akonadi/agenttype.h>
#include <akonadi/agentmanager.h>
#include <akonadi/agentinstancecreatejob.h>
#include <mailtransport/transport.h>
#include <mailtransport/transportmanager.h>

#include <KWallet/Wallet>

using namespace KWallet;
using namespace Akonadi;

CreateMail::CreateMail(KConfigGroup group, QObject* parent)
: KJob(parent)
, m_config(group)
{

}

CreateMail::~CreateMail()
{

}

void CreateMail::start()
{
    QMetaObject::invokeMethod(this, "createResource", Qt::QueuedConnection);
}

void CreateMail::createResource()
{
    const AgentType type = AgentManager::self()->type( "akonadi_imap_resource");

    AgentInstanceCreateJob *job = new AgentInstanceCreateJob( type, this );
    connect( job, SIGNAL(result(KJob*)), SLOT(instanceCreateResult(KJob*)) );
    job->start();
}

void CreateMail::instanceCreateResult(KJob* job)
{
    QString password;
    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), 0, Wallet::Synchronous);
    wallet->setFolder("WebAccounts");
    if (wallet->readPassword("google-" + m_config.name(), password) != 0) {
        qWarning("Can't open wallet");
        setError(-1);
        m_config.group("services").writeEntry("EMail", -1);
        m_config.sync();
        wallet->deleteLater();
        emitResult();
        return;
    }
    wallet->deleteLater();

    AgentInstance agent = qobject_cast<AgentInstanceCreateJob*>( job )->instance();
    agent.setName(m_config.name() + " Disconnected IMAP");

    QString service = "org.freedesktop.Akonadi.Resource." + agent.identifier();

    KConfigGroup privates(&m_config, "private");
    privates.writeEntry("emailResource", service);

    org::kde::Akonadi::Imap::Settings *settings = new org::kde::Akonadi::Imap::Settings(service, "/Settings", QDBusConnection::sessionBus());

    settings->setAuthentication(7);
    settings->setDisconnectedModeEnabled(true);
    settings->setImapPort(993);
    settings->setImapServer("imap.gmail.com");
    settings->setIntervalCheckTime(60);
    settings->setSafety("SSL");
    settings->setSubscriptionEnabled(true);
    settings->setUseDefaultIdentity(true);
    settings->setUserName(m_config.name());

    org::kde::Akonadi::Imap::Wallet *imapWallet = new org::kde::Akonadi::Imap::Wallet(service, "/Settings", QDBusConnection::sessionBus());

    imapWallet->setPassword(password);

    agent.reconfigure();

    MailTransport::Transport* mt = MailTransport::TransportManager::self()->createTransport();
    mt->setName("smtp.googlemail.com");
    mt->setHost("smtp.googlemail.com");
    mt->setPort(465);
    mt->setUserName(m_config.name());
    mt->setStorePassword(true);
    mt->setPassword(password);
    mt->setEncryption(MailTransport::Transport::EnumEncryption::SSL);
    mt->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::PLAIN );
    mt->setRequiresAuthentication(true);
    mt->writeConfig();
    MailTransport::TransportManager::self()->addTransport( mt );

    privates.writeEntry("emailTransport", mt->id());

    m_config.group("services").writeEntry("EMail", 1);
    m_config.sync();
    emitResult();
}


