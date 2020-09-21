/*
 * SPDX-FileCopyrightText: 2014 Martin Klapetek <mklapetek@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
    KAccountsDPlugin(QObject *parent, const QVariantList& args);
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

#endif // KACCOUNTSDPLUGIN_H
