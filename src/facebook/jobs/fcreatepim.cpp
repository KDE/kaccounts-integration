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

#include "fcreatepim.h"
#include "facebook_settings.h"

#include <akonadi/agenttype.h>
#include <akonadi/agentmanager.h>
#include <akonadi/agentinstancecreatejob.h>

#include <KWallet/Wallet>

using namespace KWallet;
using namespace Akonadi;

FCreatePIM::FCreatePIM(KConfigGroup group, QObject* parent)
 : KJob(parent)
 , m_config(group)
{

}

FCreatePIM::~FCreatePIM()
{

}

void FCreatePIM::start()
{
    QMetaObject::invokeMethod(this, "createResource", Qt::QueuedConnection);
}

void FCreatePIM::createResource()
{
    const AgentType type = AgentManager::self()->type( "akonadi_facebook_resource");

    AgentInstanceCreateJob *job = new AgentInstanceCreateJob( type, this );
    connect( job, SIGNAL(result(KJob*)), SLOT(instanceCreateResult(KJob*)) );
    job->start();
}

void FCreatePIM::instanceCreateResult(KJob* job)
{
    if (job->error()) {
        setError(-1);
        m_config.group("services").writeEntry("PIM", -1);
        m_config.sync();
        emitResult();
        return;
    }

    QByteArray accessToken;
    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), 0, Wallet::Synchronous);
    if (!wallet) {
        qWarning("Can't open wallet");
        setError(-1);
        m_config.group("services").writeEntry("PIM", -1);
        emitResult();
        return;
    }

    wallet->setFolder("WebAccounts");
    if (wallet->readEntry("facebookAccessToken", accessToken) != 0) {
        qWarning("Can't open wallet");
        setError(-1);
        m_config.group("services").writeEntry("PIM", -1);
        emitResult();
        return;
    }

    AgentInstance agent = qobject_cast<AgentInstanceCreateJob*>( job )->instance();
    agent.setName(m_config.name() + " Facebook");

    QString service = "org.freedesktop.Akonadi.Resource." + agent.identifier();

    KConfigGroup privates(&m_config, "private");
    privates.writeEntry("facebookResource", service);

    org::kde::Akonadi::Facebook::Settings *settings = new org::kde::Akonadi::Facebook::Settings(service, "/Settings", QDBusConnection::sessionBus());

    settings->setAccessToken(accessToken);
    settings->setUserName(m_config.name());
    settings->writeConfig();

    agent.synchronize();

    m_config.group("services").writeEntry("PIM", 1);
    emitResult();
}
