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

#include "foauth.h"
#include "facebook.h"

#include <QDebug>

#include <kfacebook/authenticationdialog.h>

FOauth::FOauth(FacebookWizard* parent)
 : QWizardPage(parent)
 , m_valid(false)
 , m_wizard(parent)
{
    setupUi(this);
}

FOauth::~FOauth()
{

}

void FOauth::initializePage()
{
    AuthenticationDialog * const authDialog = new AuthenticationDialog( this );
    authDialog->setAppId("175243235841602");
    QStringList permissions;
    permissions << "offline_access"
                << "friends_birthday"
                << "friends_website"
                << "friends_location"
                << "friends_work_history"
                << "friends_relationships"
                << "user_events"
                << "user_notes";
    authDialog->setPermissions( permissions );
    authDialog->setEnabled(true);
    authDialog->setUsername(m_wizard->username());
    authDialog->setPassword(m_wizard->password());

    connect(authDialog, SIGNAL(authenticated(QString)), this, SLOT(authenticated(QString)) );
    connect(authDialog, SIGNAL(canceled()), this, SLOT(error()) );

    authDialog->start();
}

bool FOauth::validatePage()
{
    return m_valid;
}

bool FOauth::isComplete() const
{
    return m_valid;
}

void FOauth::authenticated(const QString &accessToken)
{
    m_wizard->setAccessToken(accessToken);
    m_valid = true;

    label->setText(i18n("Authenticated"));
    Q_EMIT completeChanged();
    m_wizard->next();
}

void FOauth::error()
{
    m_valid = false;
    label->setText(i18n("Error trying to gain access"));
}