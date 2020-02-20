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

#include "accountservicetoggle.h"

#include <QDebug>
#include <KAccounts/Core>
#include <Accounts/Manager>

class AccountServiceToggle::Private {
public:
    Private() {}
    QString accountId;
    QString serviceId;
    bool serviceEnabled{false};
};

AccountServiceToggle::AccountServiceToggle(QObject* parent)
    : KJob(parent)
    , d(new Private)
{ }

AccountServiceToggle::~AccountServiceToggle()
{
    delete d;
}

QString AccountServiceToggle::accountId() const
{
    return d->accountId;
}

void AccountServiceToggle::setAccountId(const QString& accountId)
{
    d->accountId = accountId;
    Q_EMIT accountIdChanged();
}

QString AccountServiceToggle::serviceId() const
{
    return d->serviceId;
}

void AccountServiceToggle::setServiceId(const QString& serviceId)
{
    d->serviceId = serviceId;
    Q_EMIT serviceIdChanged();
}

bool AccountServiceToggle::serviceEnabled() const
{
    return d->serviceEnabled;
}

void AccountServiceToggle::setServiceEnabled(bool serviceEnabled)
{
    d->serviceEnabled = serviceEnabled;
    Q_EMIT serviceEnabledChanged();
}

void AccountServiceToggle::start()
{
    Accounts::Manager* accountsManager = KAccounts::accountsManager();
    if (accountsManager) {
        Accounts::Account *account = accountsManager->account(d->accountId.toInt());
        if (account) {
            Accounts::Service service = accountsManager->service(d->serviceId);
            if (!service.isValid()) {
//                 qWarning() << "Looks like we might have been given a name instead of an ID for the service, which will be expected when using the Ubuntu AccountServiceModel, which only gives you the name";
                for (const Accounts::Service& aService : account->services()) {
                    if (aService.displayName() == d->serviceId) {
                        service = aService;
                        break;
                    }
                }
            }
            if (service.isValid()) {
                account->selectService(service);
                account->setEnabled(d->serviceEnabled);

                if (d->serviceEnabled) {
                    account->selectService();
                    account->setEnabled(true);
                } else {
                    bool shouldStayEnabled = false;
                    for (const Accounts::Service &accountService : account->services()) {
                        // Skip the current service, that is not synced to the account yet
                        // so it would return the state before the user clicked the checkbox
                        if (accountService == service) {
                            continue;
                        }
                        account->selectService(accountService);
                        if (account->isEnabled()) {
                            // At least one service is enabled, leave the account enabled
                            shouldStayEnabled = true;
                            break;
                        }
                    }

                    // Make sure we're operating on the global account
                    account->selectService();
                    account->setEnabled(shouldStayEnabled);
                }

                connect(account, &Accounts::Account::synced, this, [this](){ emitResult(); });
                account->sync();
            } else {
                qWarning() << "No service found with the ID" << d->serviceId << "on account" << account->displayName();
                emitResult();
            }
        } else {
            qWarning() << "No account found with the ID" << d->accountId;
            emitResult();
        }
    } else {
        qWarning() << "No accounts manager, this is not awesome.";
        emitResult();
    }
}
