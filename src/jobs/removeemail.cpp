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

#include "removeemail.h"
#include "removeakonadiresource.h"

#include <mailtransport/transportmanager.h>

RemoveEmail::RemoveEmail(KConfigGroup group, QObject* parent)
: KJob(parent)
, m_config(group)
{
}

RemoveEmail::~RemoveEmail()
{

}

void RemoveEmail::start()
{
    QMetaObject::invokeMethod(this, "removeResource", Qt::QueuedConnection);
}

void RemoveEmail::removeResource()
{
    RemoveAkonadiResource *removeJob = new RemoveAkonadiResource("emailResource", m_config, this);
    connect(removeJob, SIGNAL(finished(KJob*)), this, SLOT(removeTransport()));
    removeJob->start();
}

void RemoveEmail::removeTransport()
{
    int id = m_config.group("private").readEntry("emailTransport", -1);
    if (id == -1) {
        return;
    }

    MailTransport::TransportManager::self()->removeTransport(id);

    m_config.group("services").writeEntry("EMail", 0);

    emitResult();
}
