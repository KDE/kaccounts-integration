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

#include "abstractakonadijob.h"

AbstractAkonadiJob::AbstractAkonadiJob(QObject* parent): KJob(parent)
{

}

QString AbstractAkonadiJob::resourceId() const
{
    return m_resourceId;
}

void AbstractAkonadiJob::setResourceId(const QString& resourceId)
{
    m_resourceId = resourceId;
}

QString AbstractAkonadiJob::interface() const
{
    return m_interface;
}

void AbstractAkonadiJob::setInterface(const QString& interface)
{
    m_interface = interface;
}

Accounts::AccountId AbstractAkonadiJob::accountId() const
{
    return m_accountId;
}

void AbstractAkonadiJob::setAccountId(const Accounts::AccountId& accountId)
{
    m_accountId = accountId;
}

uint qHash(const Accounts::Service &key, uint seed)
{
    return qHash(key.name() + key.provider() + key.serviceType(), seed);
}

uint qHash(const Accounts::Service &key)
{
    return qHash(key.name() + key.provider() + key.serviceType());
}