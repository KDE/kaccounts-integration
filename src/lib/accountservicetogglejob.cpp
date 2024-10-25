/*
 *  SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "accountservicetogglejob.h"

#include "core.h"
#include "debug.h"

#include <Accounts/Manager>
#include <QDebug>

namespace KAccounts
{

class AccountServiceToggleJob::Private
{
public:
    Private()
    {
    }
    QString accountId;
    QString serviceId;
    bool serviceEnabled{false};
};

AccountServiceToggleJob::AccountServiceToggleJob(QObject *parent)
    : KJob(parent)
    , d(new Private)
{
}

AccountServiceToggleJob::~AccountServiceToggleJob()
{
    delete d;
}

QString AccountServiceToggleJob::accountId() const
{
    return d->accountId;
}

void AccountServiceToggleJob::setAccountId(const QString &accountId)
{
    d->accountId = accountId;
    Q_EMIT accountIdChanged();
}

QString AccountServiceToggleJob::serviceId() const
{
    return d->serviceId;
}

void AccountServiceToggleJob::setServiceId(const QString &serviceId)
{
    d->serviceId = serviceId;
    Q_EMIT serviceIdChanged();
}

bool AccountServiceToggleJob::serviceEnabled() const
{
    return d->serviceEnabled;
}

void AccountServiceToggleJob::setServiceEnabled(bool serviceEnabled)
{
    d->serviceEnabled = serviceEnabled;
    Q_EMIT serviceEnabledChanged();
}

void AccountServiceToggleJob::start()
{
    Accounts::Manager *accountsManager = KAccounts::accountsManager();
    if (accountsManager) {
        Accounts::Account *account = accountsManager->account(d->accountId.toInt());
        if (account) {
            Accounts::Service service = accountsManager->service(d->serviceId);
            if (!service.isValid()) {
                // qCWarning(KACCOUNTS_LIB_LOG) << "Looks like we might have been given a name instead of an ID for the service, which will be expected when
                // using the Ubuntu AccountServiceModel, which only gives you the name";
                const auto services = account->services();
                for (const Accounts::Service &aService : services) {
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
                    const auto services = account->services();
                    for (const Accounts::Service &accountService : services) {
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

                connect(account, &Accounts::Account::synced, this, [this]() {
                    emitResult();
                });
                account->sync();
            } else {
                qCWarning(KACCOUNTS_LIB_LOG) << "No service found with the ID" << d->serviceId << "on account" << account->displayName();
                emitResult();
            }
        } else {
            qCWarning(KACCOUNTS_LIB_LOG) << "No account found with the ID" << d->accountId;
            emitResult();
        }
    } else {
        qCWarning(KACCOUNTS_LIB_LOG) << "No accounts manager, this is not awesome.";
        emitResult();
    }
}
}
