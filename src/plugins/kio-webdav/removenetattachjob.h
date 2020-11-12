/*
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef REMOVENETATTACHJOB_H
#define REMOVENETATTACHJOB_H

#include <KConfigGroup>
#include <KJob>

namespace KWallet
{
class Wallet;
}
class RemoveNetAttachJob : public KJob
{
    Q_OBJECT

public:
    explicit RemoveNetAttachJob(QObject *parent);
    virtual ~RemoveNetAttachJob();

    void start() override;

    QString uniqueId() const;
    void setUniqueId(const QString &uniqueId);

private Q_SLOTS:
    void removeNetAttach();
    void walletOpened(bool opened);
    void deleteDesktopFile();

private:
    QString m_uniqueId;

    KWallet::Wallet *m_wallet = nullptr;
};

#endif // REMOVENETATTACHJOB_H
