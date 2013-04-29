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

#include "akonadiservices.h"
#include "akonadiaccounts.h"

#include "jobs/lookupakonadiservices.h"
#include "jobs/removeakonadiservicesjob.h"


AkonadiServices::AkonadiServices(QObject* parent)
 : QObject(parent)
 , m_accounts(new AkonadiAccounts())
{

}

void AkonadiServices::serviceAdded(const Accounts::AccountId& accId, QMap< QString, QString >& services)
{
    LookupAkonadiServices *lookup = new LookupAkonadiServices(m_accounts, this);
    lookup->setServices(services);
    lookup->setAccountId(accId);
    lookup->start();
}

void AkonadiServices::serviceRemoved(const Accounts::AccountId& accId, QMap< QString, QString >& services)
{
    RemoveAkonadiServicesJob *job = new RemoveAkonadiServicesJob(m_accounts, this);
    job->setAccountId(accId);
    job->start();
}