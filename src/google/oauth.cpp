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

#include <libkgoogle/auth.h>
#include <libkgoogle/services/tasks.h>
#include <libkgoogle/services/contacts.h>
#include <libkgoogle/services/calendar.h>

using namespace KGoogle;

OAuth::OAuth(GoogleWizard *parent)
: QWizardPage(parent)
, m_valid(false)
{
    setupUi(this);

    m_wizard = parent;
}

OAuth::~OAuth()
{

}

void OAuth::initializePage()
{
    if (!accountExists()) {
        getTokenForAccount();
        return;
    }

    label->setText(i18n("Account already configured"));
    m_valid = true;

    Q_EMIT completeChanged();
}

bool OAuth::accountExists()
{
    Auth::instance()->init("Akonadi Google",
               "554041944266.apps.googleusercontent.com",
               "mdT1DjzohxN3npUUzkENT0gO");

    Account::Ptr acc;
    try {
        acc = Auth::instance()->getAccount(m_wizard->username());
    } catch (Exception::BaseException &e) {
        return false;
    }

    if (acc.isNull()) {
        return false;
    }

    if (acc->accessToken().isEmpty() || acc->refreshToken().isEmpty()) {
        return false;
    }

    return true;
}

void OAuth::getTokenForAccount()
{
    Auth *auth = Auth::instance();

    Account::Ptr acc(new KGoogle::Account());
    acc->addScope(Services::Tasks::ScopeUrl);
    acc->addScope(Services::Contacts::ScopeUrl);
    acc->addScope(Services::Calendar::ScopeUrl);
    acc->setAccountName(m_wizard->username());

    auth->setUsername(m_wizard->username());
    auth->setPassword(m_wizard->password());
    auth->authenticate(acc, true);

    connect(auth, SIGNAL(authenticated(KGoogle::Account::Ptr&)), this, SLOT(authenticated(KGoogle::Account::Ptr&)));
    connect(auth, SIGNAL(error(KGoogle::Error,QString)), this, SLOT(error(KGoogle::Error,QString)));
}

void OAuth::authenticated(KGoogle::Account::Ptr& acc)
{
    m_valid = true;
    label->setText(i18n("Authenticated"));
}

void OAuth::error(Error , QString )
{
    //TODO Show try again button
    m_valid = false;
    label->setText(i18n("Error trying to gain access"));
}

bool OAuth::validatePage()
{
    return m_valid;
}

bool OAuth::isComplete() const
{
    return m_valid;
}