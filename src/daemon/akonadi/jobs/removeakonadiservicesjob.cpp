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

#include "removeakonadiservicesjob.h"
#include "removeresourcejob.h"
#include "../akonadiaccounts.h"

#include <QDebug>
#include <AkonadiCore/AgentManager>

using namespace Akonadi;

RemoveAkonadiServicesJob::RemoveAkonadiServicesJob(AkonadiAccounts *accounts, QObject* parent)
: KJob(parent)
, m_accounts(accounts)
{

}

void RemoveAkonadiServicesJob::start()
{
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void RemoveAkonadiServicesJob::init()
{
    m_resources = m_accounts->resources(m_accountId);
    removeResource();
}

void RemoveAkonadiServicesJob::removeResource()
{
    qDebug();

    if (m_resources.isEmpty()) {
        m_accounts->removeAccount(m_accountId);
        return;
    }

    RemoveResourceJob *job = new RemoveResourceJob(this);
    connect(job, SIGNAL(finished(KJob*)), SLOT(removeResource()));

    job->setResourceId(m_resources.takeAt(0));
    job->start();
}

void RemoveAkonadiServicesJob::setAccountId(const Accounts::AccountId& accId)
{
    m_accountId = accId;
}
