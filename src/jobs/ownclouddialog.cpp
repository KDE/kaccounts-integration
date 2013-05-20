/*************************************************************************************
 *  Copyright (C) 2013 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#include "ownclouddialog.h"
#include "checkowncloudhostjob.h"

#include <QTimer>

#include <KDebug>
#include <KIO/Job>
#include <KGlobalSettings>
#include <kpixmapsequenceoverlaypainter.h>
#include <kpushbutton.h>


OwncloudDialog::OwncloudDialog(QWidget* parent, Qt::WindowFlags flags)
 : KDialog(parent, flags)
 , m_timerHost(new QTimer(this))
 , m_timerAuth(new QTimer(this))
 , m_hostResult(false)
 , m_authResult(false)
 , m_painter(new KPixmapSequenceOverlayPainter(this))
{
    int iconSize = IconSize(KIconLoader::MainToolbar);
    int width = QFontMetrics(KGlobalSettings::generalFont()).xHeight() * 60;

    QWidget *widget = new QWidget(this);

    setupUi(widget);
    widget->setMinimumWidth(width);
    setMainWidget(widget);

    hostWorking->setMinimumSize(iconSize, iconSize);
    passWorking->setMinimumSize(iconSize, iconSize);

    m_painter->setWidget(hostWorking);

    connect(host, SIGNAL(textChanged(QString)), SLOT(hostChanged()));
    connect(username, SIGNAL(textChanged(QString)), SLOT(authChanged()));
    connect(password, SIGNAL(textChanged(QString)), SLOT(authChanged()));

    m_timerHost->setInterval(400);
    m_timerHost->setSingleShot(true);
    m_timerAuth->setInterval(400);
    m_timerAuth->setSingleShot(true);
    connect(m_timerHost, SIGNAL(timeout()), SLOT(checkServer()));
    connect(m_timerAuth, SIGNAL(timeout()), SLOT(checkAuth()));

    button(KDialog::Ok)->setEnabled(false);
}

void OwncloudDialog::hostChanged()
{
    m_timerHost->start();
}

void OwncloudDialog::authChanged()
{
    m_timerAuth->start();
}

void OwncloudDialog::checkServer()
{
    m_url.clear();

    CheckOwncloudHostJob *job = new CheckOwncloudHostJob(this);
    connect(job, SIGNAL(finished(KJob*)), SLOT(hostChecked(KJob*)));
    job->setUrl(host->text());
    job->start();

    setWorking(true, Host);
}

void OwncloudDialog::hostChecked(KJob* job)
{
    setResult(!job->error(), Host);
    if (job->error()) {
        kDebug() << job->errorString();
        return;
    }

    CheckOwncloudHostJob *cJob = qobject_cast<CheckOwncloudHostJob*>(job);
    m_url = cJob->url();

    checkAuth();
}

void OwncloudDialog::checkAuth()
{
    if (m_url.isEmpty() || username->text().isEmpty() || password->text().isEmpty()) {
        return;
    }

    setWorking(true, Auth);

    KUrl url = m_url;
    url.setPassword(password->text());
    url.setUserName(username->text());
    url.setPath(QLatin1String("/files/webdav.php/"));

    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(0);

    connect(job, SIGNAL(finished(KJob*)), this, SLOT(authChecked(KJob*)));
}

void OwncloudDialog::authChecked(KJob* job)
{
    if (job->error()) {
        setResult(false, Auth);
        kDebug() << job->errorString();
        return;
    }

    KIO::TransferJob *tJob = qobject_cast<KIO::TransferJob*>(job);
    KIO::MetaData metadata = tJob->metaData();
    if (metadata["responsecode"] != QLatin1String("200")) {
        setResult(false, Auth);
        return;
    }

    setResult(true, Auth);
}

void OwncloudDialog::setResult(bool result, Type type)
{
    QString icon;
    if (result) {
        icon = "dialog-ok-apply";
    } else {
        icon = "dialog-close";
    }

    setWorking(false, type);
    if (type == Host) {
        m_hostResult = result;
        hostWorking->setPixmap(QIcon::fromTheme(icon).pixmap(hostWorking->sizeHint()));
    } else {
        m_authResult = result;
        passWorking->setPixmap(QIcon::fromTheme(icon).pixmap(hostWorking->sizeHint()));
    }

    if (!m_authResult || !m_authResult) {
        button(KDialog::Ok)->setEnabled(false);
    } else {
        button(KDialog::Ok)->setEnabled(true);
    }
}

void OwncloudDialog::setWorking(bool start, Type type)
{
    if (type == Auth) {
        passWorking->setPixmap(QPixmap());
        m_painter->setWidget(passWorking);
    } else {
        hostWorking->setPixmap(QPixmap());
        m_painter->setWidget(hostWorking);
    }

    if (!start) {
        m_painter->stop();;
        return;
    }

    m_painter->start();
}