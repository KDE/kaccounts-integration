/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef ACCOUNTS_DAEMON_H
#define ACCOUNTS_DAEMON_H

#include <KDEDModule>

#include <Accounts/Account>

class KAccountsDPlugin;

class KDEDAccounts : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Accounts")

public:
    explicit KDEDAccounts(QObject *parent, const QList<QVariant> &);
    ~KDEDAccounts() override;

public Q_SLOTS:
    void startDaemon();
    void accountCreated(const Accounts::AccountId id);
    void accountRemoved(const Accounts::AccountId id);
    void enabledChanged(const QString &serviceName, bool enabled);

private:
    void monitorAccount(const Accounts::AccountId id);

    QList<KAccountsDPlugin *> m_plugins;
};

#endif /*ACCOUNTS_DAEMON_H*/
