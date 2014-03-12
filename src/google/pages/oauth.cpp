/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#include "oauth.h"
#include "google/google.h"

#include <libkgapi2/account.h>
#include <libkgapi2/authjob.h>

#include <kwallet.h>
#include <KDebug>
using namespace KGAPI2;
using namespace KWallet;

OAuth::OAuth(GoogleWizard *parent)
 : QWizardPage(parent)
 , m_valid(false)
 , m_wizard(parent)
{
    setupUi(this);
    setTitle(i18n("Google Authentication"));
}

OAuth::~OAuth()
{

}

void OAuth::initializePage()
{
    QList <QWizard::WizardButton> list;
    list << QWizard::Stretch;
    list << QWizard::BackButton;
    list << QWizard::NextButton;
    list << QWizard::CancelButton;
    m_wizard->setButtonLayout(list);

    if (!accountExists()) {
        getTokenForAccount();
        return;
    }

    m_valid = true;
    label->setVisible(false);

    QMetaObject::invokeMethod(m_wizard, "next", Qt::QueuedConnection);
    Q_EMIT completeChanged();
}

bool OAuth::accountExists()
{
    m_wallet = Wallet::openWallet(Wallet::LocalWallet(), winId());
    if (!m_wallet || !m_wallet->isEnabled() || !m_wallet->isOpen()) {
        return false;
    }

    if (!m_wallet->hasFolder(QLatin1String("Akonadi Google"))) {
        return false;
    }

    if (!m_wallet->setFolder(QLatin1String("Akonadi Google"))) {
        return false;
    }
    if (!m_wallet->hasEntry(m_wizard->username())) {
        return false;
    }

    QMap<QString, QString> entries;
    if (m_wallet->readMap(m_wizard->username(), entries) != 0) {
        return false;
    }

    if (!entries.contains(QLatin1String("accessToken")) || !entries.contains(QLatin1String("refreshToken"))) {
        return false;
    }

    return true;
}

void OAuth::getTokenForAccount()
{
    AccountPtr acc(new Account);
    acc->addScope( Account::contactsScopeUrl() );
    acc->addScope( Account::calendarScopeUrl() );
    acc->addScope( Account::tasksScopeUrl() );
    acc->addScope( Account::accountInfoEmailScopeUrl() );
    acc->addScope( Account::accountInfoScopeUrl() );
    acc->setAccountName(m_wizard->username());

    AuthJob *job = new AuthJob(acc,
                               QLatin1String("554041944266.apps.googleusercontent.com"),
                               QLatin1String("mdT1DjzohxN3npUUzkENT0gO"), this);

    job->setUsername(m_wizard->username());
    job->setPassword(m_wizard->password());

    connect(job, SIGNAL(finished(KGAPI2::Job*)), SLOT(authenticated(KGAPI2::Job*)));
}

void OAuth::authenticated(KGAPI2::Job* job)
{
    AuthJob *authJob = qobject_cast<AuthJob*>(job);
    if (!authJob || authJob->error()) {
        kDebug() << job->errorString();
        setError();
        return;
    }

    AccountPtr acc = authJob->account();
    QStringList scopes;
    const QList<QUrl> urlScopes = acc->scopes();
    Q_FOREACH(const QUrl &url, urlScopes) {
        scopes << url.toString();
    }

    QMap<QString, QString> accInfo;
    accInfo.insert(QLatin1String("accessToken"), acc->accessToken());
    accInfo.insert(QLatin1String("refreshToken"), acc->refreshToken());
    accInfo.insert(QLatin1String("scopes"), scopes.join(QLatin1String(",")));

    Q_FOREACH(const QString& value, accInfo) {
        if (value.isEmpty()) {
            setError();
            return;
        }
    }

    if (!m_wallet->hasFolder(QLatin1String("Akonadi Google"))) {
        m_wallet->createFolder(QLatin1String("Akonadi Google"));
    }

    m_wallet->setFolder(QLatin1String("Akonadi Google"));
    m_wallet->writeMap(acc->accountName(), accInfo);
    label->setVisible(false);
    m_valid = true;
    m_wizard->next();
}

void OAuth::setError()
{
    label->setText(i18n("Error authenticating with Google, please press back and check your credentials"));
}

bool OAuth::validatePage()
{
    return m_valid;
}

bool OAuth::isComplete() const
{
    return m_valid;
}
