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

#include <libkgapi/auth.h>
#include <libkgapi/services/tasks.h>
#include <libkgapi/services/contacts.h>
#include <libkgapi/services/calendar.h>

#include <QDebug>
using namespace KGAPI;

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

    Account::Ptr acc(new KGAPI::Account());
    acc->addScope(Services::Tasks::ScopeUrl);
    acc->addScope(Services::Contacts::ScopeUrl);
    acc->addScope(Services::Calendar::ScopeUrl);
    acc->setAccountName(m_wizard->username());

    auth->setUsername(m_wizard->username());
    auth->setPassword(m_wizard->password());
    auth->setDialogAutoClose(true);
    auth->authenticate(acc, true);

    connect(auth, SIGNAL(authenticated(KGAPI::Account::Ptr&)), this, SLOT(authenticated(KGAPI::Account::Ptr&)));
    connect(auth, SIGNAL(error(KGAPI::Error,QString)), this, SLOT(error(KGAPI::Error,QString)));
}

void OAuth::authenticated(KGAPI::Account::Ptr& acc)
{
    m_valid = true;
    label->setVisible(false);
    m_wizard->next();
}

void OAuth::error(Error , QString err)
{
    qDebug() << err;
    m_valid = false;
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
