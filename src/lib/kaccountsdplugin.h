/*
 * Copyright 2014  Martin Klapetek <mklapetek@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef KACCOUNTSDPLUGIN_H
#define KACCOUNTSDPLUGIN_H

#include "kaccounts_export.h"

#include <QObject>
#include <Accounts/Service>
#include <Accounts/Account>

/**
 * Plugin for KAccounts daemon
 *
 * The plugin is loaded into the KAccounts daemon (a module in KDED)
 * and listens for events - new account created, account removed,
 * account service enabled and service disabled, allowing to execute
 * specialized actions with certain accounts and services.
 *
 * Each plugin should be accompanied by a provider file (if one doesn't
 * exist yet, like Google) and a service file describing the details
 * of the service. See AccountsQt docs for details.
 *
 * An example - you have an application that is working with calendars.
 * You write a plugin by extending this class. Then you add a Google
 * account into KAccounts and onAccountCreated in your plugin gets called.
 * Now your plugin must check if it is interested in the account that was added
 * and/or the services that come with it. If yes, you can set up the calendar
 * in your application in this method, using the Google OAuth token from KAccounts.
 * When the Calendar service gets disabled for Google account, your plugin
 * should disable the Google calendar in your application.
 */
class KACCOUNTS_EXPORT KAccountsDPlugin : public QObject
{
    Q_OBJECT

public:
    KAccountsDPlugin(QObject *parent = nullptr);
    virtual ~KAccountsDPlugin();

public Q_SLOTS:
    /**
     * Slot that will get called in the event of new account being created
     *
     * @param accountId Id of the newly created account (it's quint32)
     * @param serviceList List of services that the account has
     */
    virtual void onAccountCreated(const Accounts::AccountId accountId, const Accounts::ServiceList &serviceList) = 0;

    /**
     * Slot for events of account removal
     *
     * @param accountId Id of the account that got removed
     */
    virtual void onAccountRemoved(const Accounts::AccountId accountId) = 0;

    /**
     * Slot that handles account's service being enabled
     *
     * @param accountId Id of the account of which the service status changed
     * @param service The service that got enabled
     */
    virtual void onServiceEnabled(const Accounts::AccountId accountId, const Accounts::Service &service) = 0;

    /**
     * Slot that handles account's service being disabled
     *
     * @param accountId Id of the account of which the service status changed
     * @param service The service that got disabled
     */
    virtual void onServiceDisabled(const Accounts::AccountId accountId, const Accounts::Service &service) = 0;
};

Q_DECLARE_INTERFACE(KAccountsDPlugin, "org.kde.kaccounts.DPlugin")

#endif // KACCOUNTSDPLUGIN_H
