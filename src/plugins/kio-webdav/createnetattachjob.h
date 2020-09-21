/*
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CREATE_NETATTACH_H
#define CREATE_NETATTACH_H

#include <KJob>
#include <QUrl>

namespace KWallet {
    class Wallet;
}

class CreateNetAttachJob : public KJob
{
    Q_OBJECT

public:
    explicit CreateNetAttachJob(QObject *parent = nullptr);
    virtual ~CreateNetAttachJob();

    void start() override;

    QString host() const;
    void setHost(const QString &host);

    QString path() const;
    void setPath(const QString &path);

    QString realm() const;
    void setRealm(const QString &realm);

    QString name() const;
    void setName(const QString &name);

    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

    QString uniqueId() const;
    void setUniqueId(const QString &uniqueId);

    QString icon() const;
    void setIcon(const QString &icon);

private Q_SLOTS:
    void createNetAttach();
    void walletOpened(bool opened);
    void gotRealm(KJob *job);

private:
    void getRealm();
    void createDesktopFile(const QUrl &url);

    QString m_host;
    QString m_path;
    QString m_realm;
    QString m_name;
    QString m_username;
    QString m_password;
    QString m_uniqueId;
    QString m_icon;

    KWallet::Wallet *m_wallet = nullptr;
};

#endif //CREATE_NETATTACH_H
