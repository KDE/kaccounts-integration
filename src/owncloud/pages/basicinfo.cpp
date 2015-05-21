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

#include "basicinfo.h"
#include "owncloud/owncloud.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

#include <kpixmapsequenceoverlaypainter.h>
#include <KIO/Job>
#include <KIO/global.h>

BasicInfo::BasicInfo(OwnCloudWizard* parent)
 : QWizardPage(parent)
 , m_validServer(false)
 , m_painter(new KPixmapSequenceOverlayPainter(this))
 , m_wizard(parent)
{
    setupUi(this);
    icon->setPixmap(QIcon::fromTheme("owncloud").pixmap(32, 32));

    int lineEditHeight = server->sizeHint().height();
    QSize workingSize(lineEditHeight, lineEditHeight);
    working->setMinimumSize(workingSize);

    m_painter->setWidget(working);

    username->setFocus();
    connect(server, SIGNAL(textChanged(QString)), this, SLOT(checkServer()));
    connect(username, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    connect(password, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
}

BasicInfo::~BasicInfo()
{

}

void BasicInfo::initializePage()
{
    QList <QWizard::WizardButton> list;
    list << QWizard::Stretch;
    list << QWizard::NextButton;
    list << QWizard::CancelButton;
    m_wizard->setButtonLayout(list);
}

bool BasicInfo::validatePage()
{
    if (!validData()) {
        return false;
    }

    m_wizard->setUsername(username->text());
    m_wizard->setPassword(password->text());
    m_wizard->setServer(m_server);

    return true;
}

bool BasicInfo::isComplete() const
{
    return validData();
}

void BasicInfo::checkServer()
{
    checkServer(server->text());
}

bool BasicInfo::validData() const
{
    if (username->text().isEmpty() || password->text().isEmpty()) {
        return false;
    }

    if (!m_validServer) {
        return false;
    }

    return true;
}

void BasicInfo::checkServer(const QString &path)
{
    QString fixedUrl;
    if (!path.startsWith("http://") && !path.startsWith("https://")) {
        fixedUrl.append("https://");
        fixedUrl.append(path);
    } else {
        fixedUrl = path;
    }

    m_json.clear();

    QUrl url(fixedUrl);
    url = url.adjusted(QUrl::StripTrailingSlash);
    url.setPath(url.path() + '/' + "status.php");

    checkServer(url);
}

void BasicInfo::checkServer(const QUrl &url)
{
    qDebug() << url;
    setResult(false);
    setWorking(true);
    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(0);
    connect(job, SIGNAL(data(KIO::Job*,QByteArray)), SLOT(dataReceived(KIO::Job*,QByteArray)));
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(fileChecked(KJob*)));
}


void BasicInfo::figureOutServer(const QString& urlStr)
{
    if (urlStr == QLatin1String("/") || urlStr.isEmpty()) {
        setResult(false);
        return;
    }

    m_json.clear();

    QUrl url(urlStr);
    url = KIO::upUrl(urlStr);
    url.setPath(url.path() + '/' + "status.php");

    checkServer(url.adjusted(QUrl::NormalizePathSegments));
}

void BasicInfo::dataReceived(KIO::Job* job, const QByteArray& data)
{
    m_json.append(data);
}

void BasicInfo::fileChecked(KJob* job)
{
    KIO::TransferJob *kJob = qobject_cast<KIO::TransferJob *>(job);
    if (kJob->error()) {
        qDebug() << job->errorString();
        qDebug() << job->errorText();
        figureOutServer(kJob->url().url());
        return;
    }

    QJsonDocument parser = QJsonDocument::fromJson(m_json);
    QJsonObject map = parser.object();
    if (!map.contains("version")) {
        figureOutServer(kJob->url().url());
        return;
    }

    m_server = kJob->url();
    m_server.setFileName("");
    qDebug() << m_server;
    setResult(true);

    Q_EMIT completeChanged();
}

void BasicInfo::setWorking(bool start)
{
    working->setPixmap(QPixmap());

    if (!start) {
        m_painter->stop();;
        return;
    }

    m_painter->start();
}

void BasicInfo::setResult(bool result)
{
    QString icon;
    if (result) {
        icon = "dialog-ok-apply";
    } else {
        icon = "dialog-close";
    }

    m_validServer = result;
    setWorking(false);
    working->setPixmap(QIcon::fromTheme(icon).pixmap(working->sizeHint()));
}