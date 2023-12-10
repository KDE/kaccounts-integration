/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIO_SERVICES_H
#define KIO_SERVICES_H

#include "kaccountsdplugin.h"

#include <QMap>
#include <QObject>
#include <QString>

#include <Accounts/Account>
#include <Accounts/Service>

#include <QCoroCore>

class KJob;
class AkonadiAccounts;

namespace Accounts
{
class Manager;
}

class KIOServices : public KAccounts::KAccountsDPlugin
{
    Q_OBJECT

public:
    KIOServices(QObject *parent, const QVariantList &args);
    ~KIOServices() override;

public Q_SLOTS:
    void onAccountCreated(const Accounts::AccountId accId, const Accounts::ServiceList &serviceList) override;
    void onAccountRemoved(const Accounts::AccountId accId) override;
    void onServiceEnabled(const Accounts::AccountId accId, const Accounts::Service &service) override;
    void onServiceDisabled(const Accounts::AccountId accId, const Accounts::Service &service) override;

private:
    void enableService(const Accounts::AccountId accId, const Accounts::Service &service);
    void disableService(const Accounts::AccountId accId, const QString &serviceName);
    QCoro::Task<void> createNetAttach(const Accounts::AccountId accId, const Accounts::Service &service);
    QCoro::Task<QString> getRealm(const QUrl &url);
    QCoro::Task<void> removeNetAttach(const QString &id);
    bool isEnabled(const Accounts::AccountId accId, const QString &serviceName);
};

#endif // KIO_SERVICES_H
