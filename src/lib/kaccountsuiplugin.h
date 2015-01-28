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

#ifndef KACCOUNTSUIPLUGIN_H
#define KACCOUNTSUIPLUGIN_H

#include "kaccounts_export.h"

#include <QObject>

class KACCOUNTS_EXPORT KAccountsUiPlugin : public QObject
{
    Q_OBJECT

public:
    enum UiType {
        NewAccountDialog,
        ConfigureAccountDialog
    };


    KAccountsUiPlugin(QObject *parent = 0);
    virtual ~KAccountsUiPlugin();

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

Q_SIGNALS:
    /**
     * Sometimes the plugins might take time to initialize the UI
     * completely, whenever they are ready, this signal should be
     * emitted to tell kaccounts that the plugin is ready to display
     * the dialog
     */
    void uiReady();
    /**
     * Emitted when user successfully authenticated using this plugin
     * The params are the username & password that the user used to
     * authenticate himself and any additional data that might be needed
     */
    void success(const QString &username, const QString &password, const QVariantMap &additionalData);

    /**
     * Emitted when there has been an error during the authentication
     *
     * @param errorString The error that has occured
     */
    void error(const QString &errorString);
};

Q_DECLARE_INTERFACE(KAccountsUiPlugin, "org.kde.kaccounts.UiPlugin")

#endif // KACCOUNTSUIPLUGIN_H
