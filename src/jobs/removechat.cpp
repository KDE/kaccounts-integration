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

#include "removechat.h"

#include <TelepathyQt/Types>
#include <TelepathyQt/Account>
#include <TelepathyQt/PendingReady>

RemoveChat::RemoveChat(KConfigGroup& group, QObject* parent)
: KJob(parent)
, m_config(group)
{

}


RemoveChat::~RemoveChat()
{

}

void RemoveChat::start()
{
    QMetaObject::invokeMethod(this, "removeResource", Qt::QueuedConnection);
}

void RemoveChat::removeResource()
{
    m_manager = Tp::AccountManager::create(QDBusConnection::sessionBus());

    connect(m_manager->becomeReady(),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onAccountManagerReady(Tp::PendingOperation*)));
}

void RemoveChat::onAccountManagerReady(Tp::PendingOperation* op)
{
    QString accID = m_config.group("private").readEntry<QString>("chatUID", "");
//     m_manager->accountForPath(accID)->remove();

    QList <Tp::AccountPtr > accounts =  m_manager->allAccounts();

    Q_FOREACH(Tp::AccountPtr acc, accounts) {
        if (acc->uniqueIdentifier() == accID) {
            acc->remove();
            m_config.group("private").deleteEntry("chatUID");
            break;
        }
    }

    emitResult();
}
