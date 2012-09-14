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
#include "facebook/facebook.h"

#include <qjson/parser.h>

#include <KPixmapSequenceOverlayPainter>

#include <kfacebook/authenticationdialog.h>

#include <KDebug>
#include <KIO/Job>

using namespace KFacebook;
FOauth::FOauth(FacebookWizard* parent)
 : QWizardPage(parent)
 , m_valid(false)
 , m_wizard(parent)
 , m_painter(new KPixmapSequenceOverlayPainter(this))
{
    setupUi(this);
    setTitle(i18n("Facebook Authentication"));

    m_painter->setWidget(working);
    m_painter->start();

    tryAgain->setVisible(false);
    connect(tryAgain, SIGNAL(clicked(bool)), this, SLOT(tryAgainSlot()));
}

FOauth::~FOauth()
{

}

void FOauth::initializePage()
{
    QList <QWizard::WizardButton> list;
    list << QWizard::Stretch;
    list << QWizard::BackButton;
    list << QWizard::NextButton;
    list << QWizard::CancelButton;
    m_wizard->setButtonLayout(list);

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
                << "user_notes"
                << "user_about_me";
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
    sender()->disconnect();
    m_wizard->setAccessToken(accessToken);

    checkUsername();
}

void FOauth::error()
{
    m_painter->stop();
    working->setVisible(false);
    m_valid = false;
    label->setText(i18n("Error authenticating with Facebook, please press back and check your credentials"));
}

void FOauth::gotUsername(KIO::Job *job, const QByteArray &data)
{
    m_json.append(data);
}

void FOauth::usernameFinished()
{
    m_painter->stop();
    working->setVisible(false);

    QJson::Parser parser;
    QMap <QString, QVariant > data = parser.parse(m_json).toMap();;

    if (!data.contains("username") || data["username"].toString().isEmpty()) {
        tryAgain->setVisible(true);
        tryAgain->setDisabled(false);
        label->setText("A facebook username is needed to connect external applications to it.<br/> Use <a href=\"http://www.facebook.com/username/\">this page</a> to choose one.");
        return;
    }

    QString username = data["username"].toString();

    m_valid = true;
    m_wizard->setFacebookUsername(username);

    Q_EMIT completeChanged();
    m_wizard->next();
}

void FOauth::checkUsername()
{
    label->setText(i18n("Checking username..."));
    tryAgain->setDisabled(true);

    KUrl url("https://graph.facebook.com/me");
    url.addQueryItem("access_token", m_wizard->accessToken());

    kDebug() << url;
    KIO::TransferJob* job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    connect(job, SIGNAL(data(KIO::Job*,QByteArray)), this, SLOT(gotUsername(KIO::Job*, QByteArray)));
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(usernameFinished()));
    job->start();
}

void FOauth::tryAgainSlot()
{
    checkUsername();
}
