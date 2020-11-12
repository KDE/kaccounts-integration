/*
 *  SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef ACCOUNTSERVICETOGGLE_H
#define ACCOUNTSERVICETOGGLE_H

#include "kaccounts_export.h"

#include <kjob.h>

#include <QStringList>

/**
 * @brief A job used to change the enabled state of a specific service on a specific account
 */
class KACCOUNTS_EXPORT AccountServiceToggleJob : public KJob
{
    Q_OBJECT
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
    Q_PROPERTY(QString serviceId READ serviceId WRITE setServiceId NOTIFY serviceIdChanged)
    Q_PROPERTY(bool serviceEnabled READ serviceEnabled WRITE setServiceEnabled NOTIFY serviceEnabledChanged)
public:
    explicit AccountServiceToggleJob(QObject *parent = nullptr);
    virtual ~AccountServiceToggleJob();

    void start() override;

    QString accountId() const;
    void setAccountId(const QString &accountId);
    Q_SIGNAL void accountIdChanged();

    QString serviceId() const;
    void setServiceId(const QString &serviceId);
    Q_SIGNAL void serviceIdChanged();

    bool serviceEnabled() const;
    void setServiceEnabled(bool serviceEnabled);

Q_SIGNALS:
    void serviceEnabledChanged();

private:
    class Private;
    Private *d;
};
#endif // ACCOUNTSERVICETOGGLE_H
