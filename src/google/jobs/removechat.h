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

#ifndef REMOVECHAT_H
#define REMOVECHAT_H

#include <TelepathyQt/AccountManager>

#include <kconfiggroup.h>
#include <KDE/KJob>

namespace Tp {
    class PendingOperation;
}
class RemoveChat : public KJob
{
    Q_OBJECT
    public:
        explicit RemoveChat(KConfigGroup &group, QObject* parent = 0);
        virtual ~RemoveChat();

        virtual void start();

    private Q_SLOTS:
        void removeResource();
        void onAccountManagerReady(Tp::PendingOperation *op);

    private:
        KConfigGroup m_config;
        Tp::AccountManagerPtr m_manager;
};

#endif //REMOVECHAT_H