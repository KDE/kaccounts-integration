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

#include <qjson/parser.h>

#include <KDebug>
#include <KIO/Job>
#include <KGlobalSettings>
#include <kpixmapsequenceoverlaypainter.h>


OwncloudDialog::OwncloudDialog(QWidget* parent, Qt::WindowFlags flags)
 : KDialog(parent, flags)
 , m_validHost(false)
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

    connect(host, SIGNAL(textChanged(QString)), SLOT(checkServer(QString)));
    connect(username, SIGNAL(textChanged(QString)), SLOT(checkAuth()));
    connect(password, SIGNAL(textChanged(QString)), SLOT(checkAuth()));
}

void OwncloudDialog::checkServer(const QString &path)
{
    QString fixedUrl;
    if (!path.startsWith("http://") && !path.startsWith("https://")) {
        fixedUrl.append("http://");
        fixedUrl.append(path);
    } else {
        fixedUrl = path;
    }

    KUrl url(fixedUrl);
    m_json.clear();

    url.setFileName("status.php");

    checkServer(url);
}

void OwncloudDialog::checkServer(const KUrl& url)
{
    kDebug() << url;
    setWorking(true);

    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(0);

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)), SLOT(dataReceived(KIO::Job*,QByteArray)));
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(fileChecked(KJob*)));
}

void OwncloudDialog::dataReceived(KIO::Job* job, const QByteArray& data)
{
    m_json.append(data);
}

void OwncloudDialog::fileChecked(KJob* job)
{
    KIO::TransferJob *kJob = qobject_cast<KIO::TransferJob *>(job);
    if (kJob->error()) {
        kDebug() << job->errorString();
        kDebug() << job->errorText();
        figureOutServer(kJob->url().url());
        return;
    }

    QJson::Parser parser;
    QMap <QString, QVariant> map = parser.parse(m_json).toMap();
    if (!map.contains("version")) {
        figureOutServer(kJob->url().url());
        return;
    }

    m_server = kJob->url();
    m_server.setFileName("");
    kDebug() << m_server;
    setResult(true);

    checkAuth();
}

void OwncloudDialog::figureOutServer(const QString& urlStr)
{
    KUrl url(urlStr);
    if (url.directory(KUrl::AppendTrailingSlash) == "/") {
        setResult(false);
        return;
    }

    m_json.clear();
    url.setFileName("");
    url = url.upUrl();
    url.setFileName("status.php");

    checkServer(url);
}

void OwncloudDialog::checkAuth()
{
    if (!m_validHost || username->text().isEmpty() || password->text().isEmpty()) {
        return;
    }

    setWorking(true);

    KUrl url = m_server;
    url.setPassword(password->text());
    url.setUserName(username->text());

    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(0);

    connect(job, SIGNAL(finished(KJob*)), this, SLOT(authChecked(KJob*)));
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

    m_validHost = result;
    setWorking(false);
    if (!m_validHost) {
        hostWorking->setPixmap(QIcon::fromTheme(icon).pixmap(hostWorking->sizeHint()));
    } else {
        passWorking->setPixmap(QIcon::fromTheme(icon).pixmap(hostWorking->sizeHint()));
    }
}

void OwncloudDialog::setWorking(bool start)
{
    if (m_validHost) {
        passWorking->setPixmap(QPixmap());
        m_painter->setWidget(passWorking);
    } else {
        hostWorking->setPixmap(QPixmap());
    }

    if (!start) {
        m_painter->stop();;
        return;
    }

    m_painter->start();
}