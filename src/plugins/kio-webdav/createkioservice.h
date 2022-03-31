/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CREATE_KIOSERVICE_H
#define CREATE_KIOSERVICE_H

#include <KJob>

#include <Accounts/Account>

namespace Accounts
{
class Manager;
}

namespace KWallet
{
class Wallet;
}

class CreateKioService : public KJob
{
    Q_OBJECT

public:
    explicit CreateKioService(QObject *parent = nullptr);
    ~CreateKioService() override;

    void start() override;

    Accounts::AccountId accountId() const;
    void setAccountId(const Accounts::AccountId accId);

    QString serviceName() const;
    void setServiceName(const QString &serviceName);

    QString serviceType() const;
    void setServiceType(const QString &serviceType);

private Q_SLOTS:
    void createKioService();
    void gotCredentials(KJob *job);
    void netAttachCreated(KJob *job);

private:
    void createDesktopFile();

    Accounts::Manager *m_manager = nullptr;
    Accounts::Account *m_account = nullptr;
    Accounts::AccountId m_accountId;
    QString m_serviceName;
    QString m_serviceType;
};

#endif // CREATE_KIOSERVICE_H
