/*
 *  SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef REMOVEACCOUNT_H
#define REMOVEACCOUNT_H

#include "kaccounts_export.h"

#include <kjob.h>

#include <QString>

/**
 * @brief A job which will attempt to remove the specified account
 */
class KACCOUNTS_EXPORT RemoveAccountJob : public KJob
{
    Q_OBJECT
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
public:
    explicit RemoveAccountJob(QObject *parent = nullptr);
    ~RemoveAccountJob() override;

    void start() override;

    QString accountId() const;
    void setAccountId(const QString &accountId);
    Q_SIGNAL void accountIdChanged();

private:
    class Private;
    Private *d;
};
#endif // REMOVEACCOUNT_H
