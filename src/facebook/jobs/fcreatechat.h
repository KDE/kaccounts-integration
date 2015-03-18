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

#ifndef F_CREATE_CHAT_H
#define F_CREATE_CHAT_H

#include <TelepathyQt/AccountManager>

#include <KJob>
#include <KConfigGroup>

class FCreateChat : public KJob
{
    Q_OBJECT
    public:
        explicit FCreateChat(KConfigGroup &group, QObject* parent = 0);
        virtual ~FCreateChat();

        virtual void start();

    public Q_SLOTS:
        void createAccount();
        void onAccountManagerReady(Tp::PendingOperation *op);
        void onAccountCreated(Tp::PendingOperation *op);

    private:
        KConfigGroup m_config;
        Tp::AccountManagerPtr m_manager;
};

#endif //F_CREATE_CHAT_H