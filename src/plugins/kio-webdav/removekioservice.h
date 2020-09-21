/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef REMOVE_KIOSERVICE_H
#define REMOVE_KIOSERVICE_H

#include <KJob>

#include <Accounts/Account>

class RemoveKioService : public KJob
{
    Q_OBJECT

public:
    explicit RemoveKioService(QObject *parent = nullptr);

    void start() override;

    Accounts::AccountId accountId() const;
    void setAccountId(Accounts::AccountId accId);

    QString serviceName() const;
    void setServiceName(const QString &serviceName);

private Q_SLOTS:
    void removeKioService();
    void removeNetAatachFinished(KJob *job);

private:
    Accounts::AccountId m_accountId;
    QString m_serviceName;
};

#endif //REMOVE_KIOSERVICE_H
