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

#include "credentials.h"
#include "google/google.h"

#include <QLabel>
#include <QRegExpValidator>

#include <KColorScheme>
#include <kpimutils/email.h>
#include <kpimutils/emailvalidator.h>

Credentials::Credentials(GoogleWizard* parent)
 : QWizardPage(parent)
 , m_completed(false)
 , m_wizard(parent)
{
    setupUi(this);
    setTitle(i18n("Credentials"));
    googleIcon->setPixmap(QIcon::fromTheme("gmail").pixmap(32, 32));

    QList <QWizard::WizardButton> list;
    list << QWizard::Stretch;
    list << QWizard::NextButton;
    list << QWizard::CancelButton;
    m_wizard->setButtonLayout(list);

    KColorScheme scheme(QPalette::Normal);
    KColorScheme::ForegroundRole role;

    QPalette palette(error->palette());
    palette.setColor(QPalette::Foreground, scheme.foreground(KColorScheme::NegativeText).color());

    error->setPalette(palette);

    KPIMUtils::EmailValidator* emailValidator = new KPIMUtils::EmailValidator( this );
    email->setValidator( emailValidator );

    connect(email, SIGNAL(textChanged(QString)), SLOT(validateForm()));
    connect(password, SIGNAL(textChanged(QString)), SLOT(validateForm()));
}

Credentials::~Credentials()
{

}

bool Credentials::validatePage()
{
    QString errorString;
    if (email->text().isEmpty() && password->text().isEmpty()) {
        errorString.append(i18n("The email and password are required"));
        error->setText(errorString);
        return false;
    }

    if (email->text().isEmpty()) {
        errorString.append(i18n("The email is required"));
    }

    int a = 0;
    QString str;
    if (!KPIMUtils::isValidSimpleAddress(email->text())) {
        str = i18n("The email seems to be wrong (Invalid format)");
        !errorString.isEmpty() ? errorString.append("\n" + str) : errorString.append(str);
    }

    if (password->text().isEmpty()) {
        str = i18n("The password is required");
        !errorString.isEmpty() ? errorString.append("\n" + str) : errorString.append(str);
    }

    if (!errorString.isEmpty()) {
        error->setText(errorString);
        return false;
    }

    error->setText("");
    m_wizard->setUsername(email->text());
    m_wizard->setPassword(password->text());
    return true;
}

bool Credentials::isComplete() const
{
    return m_completed;
}

void Credentials::validateForm()
{
    m_completed = validatePage();
    Q_EMIT completeChanged();
}