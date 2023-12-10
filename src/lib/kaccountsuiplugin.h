/*
 * SPDX-FileCopyrightText: 2014 Martin Klapetek <mklapetek@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef KACCOUNTSUIPLUGIN_H
#define KACCOUNTSUIPLUGIN_H

#include "kaccounts_export.h"

#include <QObject>

class QWindow;

namespace KAccounts
{

class KACCOUNTS_EXPORT KAccountsUiPlugin : public QObject
{
    Q_OBJECT

public:
    enum UiType {
        NewAccountDialog,
        ConfigureAccountDialog,
    };

    explicit KAccountsUiPlugin(QObject *parent = nullptr);
    ~KAccountsUiPlugin() override;

    virtual void init(UiType type) = 0;

    /**
     * Sets the selected Accounts-SSO provider to the plugin
     */
    virtual void setProviderName(const QString &providerName) = 0;

    /**
     * Called when the dialog for creating new account should show
     */
    virtual void showNewAccountDialog() = 0;

    /**
     * Called when an existing account should be configured
     * @param accountId The ID of the account that should be configured
     */
    virtual void showConfigureAccountDialog(const quint32 accountId) = 0;

    /**
     * Returns a list of services which this plugin supports
     * configuration of, for example "IM" supports config
     * of IM/KTp accounts
     */
    virtual QStringList supportedServicesForConfig() const = 0;

    QWindow *transientParent() const;

Q_SIGNALS:
    /**
     * Sometimes the plugins might take time to initialize the UI
     * completely, whenever they are ready, this signal should be
     * emitted to tell kaccounts that the plugin is ready to display
     * the dialog
     */
    void uiReady();

    /**
     * This should be emitted when the plugin finishes building the UI
     * for configuring the selected account
     */
    void configUiReady();

    /**
     * Emitted when user successfully authenticated using this plugin
     * The params are the username & password that the user used to
     * authenticate themselves and any additional data that might be needed
     */
    void success(const QString &username, const QString &password, const QVariantMap &additionalData);

    /**
     * Emit this to start an auth session. Any values in data are passed to the started session.
     * This is useful e.g. when doing oauth login but you need to show a UI to get some data before.
     */
    void startAuthSession(const QVariantMap &data);

    /**
     * Emitted when there has been an error during the authentication
     *
     * @param errorString The error that has occurred
     */
    void error(const QString &errorString);

    /**
     * Emitted when the user cancels the account creation
     */
    void canceled();
};

};

Q_DECLARE_INTERFACE(KAccounts::KAccountsUiPlugin, "org.kde.kaccounts.UiPlugin")

#endif // KACCOUNTSUIPLUGIN_H
