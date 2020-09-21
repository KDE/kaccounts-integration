/*
 *  SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "changeaccountdisplaynamejob.h"

#include "core.h"
#include <Accounts/Manager>
#include <KLocalizedString>
#include <QDebug>

class ChangeAccountDisplayNameJob::Private {
public:
    Private() {}
    QString accountId;
    QString displayName;
};

ChangeAccountDisplayNameJob::ChangeAccountDisplayNameJob(QObject* parent)
    : KJob(parent)
    , d(new Private)
{ }

ChangeAccountDisplayNameJob::~ChangeAccountDisplayNameJob()
{
    delete d;
}

QString ChangeAccountDisplayNameJob::accountId() const
{
    return d->accountId;
}

void ChangeAccountDisplayNameJob::setAccountId(const QString& accountId)
{
    d->accountId = accountId;
    Q_EMIT accountIdChanged();
}

QString ChangeAccountDisplayNameJob::displayName() const
{
    return d->displayName;
}

void ChangeAccountDisplayNameJob::setDisplayName(const QString& displayName)
{
    d->displayName = displayName;
    Q_EMIT displayNameChanged();
}

void ChangeAccountDisplayNameJob::start()
{
    if (!d->displayName.isEmpty()) {
        Accounts::Manager* accountsManager = KAccounts::accountsManager();
        if (accountsManager) {
            Accounts::Account *account = accountsManager->account(d->accountId.toInt());
            if (account) {
                account->setDisplayName(d->displayName);
                connect(account, &Accounts::Account::synced, this, [this](){ emitResult(); });
                account->sync();
            } else {
                qWarning() << "No account found with the ID" << d->accountId;
                setErrorText(i18n("No account found with the ID %1").arg(d->accountId));
                emitResult();
            }
        } else {
            qWarning() << "No accounts manager, this is not awesome.";
            setErrorText(i18n("No accounts manager, this is not awesome."));
            emitResult();
        }
    } else {
        qWarning() << "Setting an account display name to empty is a terrible idea, and we refuse to do that";
        setErrorText(i18n("The display name cannot be empty"));
        emitResult();
    }
}
