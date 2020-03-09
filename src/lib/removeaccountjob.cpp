/*************************************************************************************
 *  Copyright (C) 2020 by Dan Leinir Turthra Jensen <admin@leinir.dk>                *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include "removeaccountjob.h"

#include <QDebug>
#include "core.h"
#include <Accounts/Manager>
#include <SignOn/Identity>

class RemoveAccountJob::Private {
public:
    Private() {}
    QString accountId;
};

RemoveAccountJob::RemoveAccountJob(QObject* parent)
    : KJob(parent)
    , d(new Private)
{ }

RemoveAccountJob::~RemoveAccountJob()
{
    delete d;
}

QString RemoveAccountJob::accountId() const
{
    return d->accountId;
}

void RemoveAccountJob::setAccountId(const QString& accountId)
{
    d->accountId = accountId;
    Q_EMIT accountIdChanged();
}

void RemoveAccountJob::start()
{
    Accounts::Manager* accountsManager = KAccounts::accountsManager();
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
