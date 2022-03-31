/*
 *  SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CHANGEACCOUNTDISPLAYNAMEJOB_H
#define CHANGEACCOUNTDISPLAYNAMEJOB_H

#include "kaccounts_export.h"

#include <kjob.h>

#include <QStringList>

/**
 * @brief A job used to change the human-readable name of a specified account
 *
 * This job will refuse to change the name to something empty (while it is technically
 * possible to do so for an account, it is highly undesirable)
 */
class KACCOUNTS_EXPORT ChangeAccountDisplayNameJob : public KJob
{
    Q_OBJECT
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
public:
    explicit ChangeAccountDisplayNameJob(QObject *parent = nullptr);
    ~ChangeAccountDisplayNameJob() override;

    void start() override;

    QString accountId() const;
    void setAccountId(const QString &accountId);
    Q_SIGNAL void accountIdChanged();

    QString displayName() const;
    void setDisplayName(const QString &displayName);
    Q_SIGNAL void displayNameChanged();

private:
    class Private;
    Private *d;
};
#endif // CHANGEACCOUNTDISPLAYNAMEJOB_H
