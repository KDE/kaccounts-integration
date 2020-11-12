/*
 *  SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "removeaccountjob.h"

#include "core.h"
#include <Accounts/Manager>
#include <QDebug>
#include <SignOn/Identity>

class RemoveAccountJob::Private
{
public:
    Private()
    {
    }
    QString accountId;
};

RemoveAccountJob::RemoveAccountJob(QObject *parent)
    : KJob(parent)
    , d(new Private)
{
}

RemoveAccountJob::~RemoveAccountJob()
{
    delete d;
}

QString RemoveAccountJob::accountId() const
{
    return d->accountId;
}

void RemoveAccountJob::setAccountId(const QString &accountId)
{
    d->accountId = accountId;
    Q_EMIT accountIdChanged();
}

void RemoveAccountJob::start()
{
    Accounts::Manager *accountsManager = KAccounts::accountsManager();
    if (accountsManager) {
        Accounts::Account *account = accountsManager->account(d->accountId.toInt());
        if (account) {
            // We can't depend on Accounts::Account::synced, as that doesn't necessarily get fired when
            // asking for the account to be removed...
            connect(accountsManager, &Accounts::Manager::accountRemoved, this, [this](Accounts::AccountId id) {
                if (id == d->accountId.toUInt()) {
                    emitResult();
                }
            });
            SignOn::Identity *identity = SignOn::Identity::existingIdentity(account->credentialsId(), this);
            if (identity) {
                identity->remove();
                identity->deleteLater();
            }
            account->remove();
            account->sync();
        } else {
            qWarning() << "No account found with the ID" << d->accountId;
            emitResult();
        }
    } else {
        qWarning() << "No accounts manager, this is not awesome.";
        emitResult();
    }
}
