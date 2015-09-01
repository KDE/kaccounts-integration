/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
 *  Copyright (C) 2015 by Martin Klapetek <mklapetek@kde.org>                        *
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

#ifndef OWNCLOUD_H
#define OWNCLOUD_H

#include <QUrl>
#include <QHash>

#include "../lib/kaccountsuiplugin.h"

class QWizard;

class OwnCloudWizard : public KAccountsUiPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kaccounts.UiPlugin")
    Q_INTERFACES(KAccountsUiPlugin)

public:
    explicit OwnCloudWizard(QWidget *parent = 0);
    virtual ~OwnCloudWizard();

    virtual void init(KAccountsUiPlugin::UiType type) Q_DECL_OVERRIDE;
    virtual void setProviderName(const QString &providerName) Q_DECL_OVERRIDE;
    virtual void showNewAccountDialog() Q_DECL_OVERRIDE;
    virtual void showConfigureAccountDialog(const quint32 accountId) Q_DECL_OVERRIDE;
    virtual QStringList supportedServicesForConfig() const Q_DECL_OVERRIDE;

    void done();

    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setServer(const QUrl &server);

    const QString username() const;
    const QString password() const;
    const QUrl server() const;

    void activateOption(const QString &name, bool checked);

Q_SIGNALS:
    void newAccount(const QString &type, const QString &name);

private:
    QWizard *m_wizard;
    QString m_username;
    QString m_password;
    QUrl m_server;
    QString m_providerName;

    QHash<QString, int> m_services;
};

#endif //OWNCLOUD_H
