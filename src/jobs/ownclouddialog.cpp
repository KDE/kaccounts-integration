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
#include <KGlobalSettings>
#include <kpixmapsequenceoverlaypainter.h>


OwncloudDialog::OwncloudDialog(QWidget* parent, Qt::WindowFlags flags)
 : KDialog(parent, flags)
 , m_timer(new QTimer(this))
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
    connect(username, SIGNAL(textChanged(QString)), SLOT(checkAuth()));
    connect(password, SIGNAL(textChanged(QString)), SLOT(checkAuth()));

    m_timer->setInterval(400);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), SLOT(checkServer()));
}

void OwncloudDialog::hostChanged()
{
    m_timer->start();
}

void OwncloudDialog::checkServer()
{
    CheckOwncloudHostJob *job = new CheckOwncloudHostJob(this);
    connect(job, SIGNAL(finished(KJob*)), SLOT(hostChecked(KJob*)));
    job->setUrl(host->text());
    job->start();

    setWorking(true);
}

void OwncloudDialog::hostChecked(KJob* job)
{
    if (job->error()) {
        kDebug() << job->errorString();
    }

    setResult(!job->error());
}

void OwncloudDialog::checkAuth()
{
    if (username->text().isEmpty() || password->text().isEmpty()) {
        return;
    }
/*
    setWorking(true);

    KUrl url = m_server;
    url.setPassword(password->text());
    url.setUserName(username->text());

    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(0);

    connect(job, SIGNAL(finished(KJob*)), this, SLOT(authChecked(KJob*)));*/
}

void OwncloudDialog::authChecked(KJob* job)
{
    if (job->error()) {
        setResult(false);
        return;
    }

    setResult(true);
}

void OwncloudDialog::setResult(bool result)
{
    QString icon;
    if (result) {
        icon = "dialog-ok-apply";
    } else {
        icon = "dialog-close";
    }

//     m_validHost = result;
    setWorking(false);
//     if (!m_validHost) {
        hostWorking->setPixmap(QIcon::fromTheme(icon).pixmap(hostWorking->sizeHint()));
//     } else {
//         passWorking->setPixmap(QIcon::fromTheme(icon).pixmap(hostWorking->sizeHint()));
//     }
}

void OwncloudDialog::setWorking(bool start)
{
//     if (m_validHost) {
//         passWorking->setPixmap(QPixmap());
//         m_painter->setWidget(passWorking);
//     } else {
        hostWorking->setPixmap(QPixmap());
//     }

    if (!start) {
        m_painter->stop();;
        return;
    }

    m_painter->start();
}