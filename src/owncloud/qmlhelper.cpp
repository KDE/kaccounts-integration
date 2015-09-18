/*************************************************************************************
 *  Copyright (C) 2015 by Martin Klapetek <mklapetek@kde.org>                        *
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

#include "qmlhelper.h"

#include <KIO/Job>
#include <KIO/DavJob>
#include <kio/global.h>
#include <KLocalizedString>

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

QmlHelper::QmlHelper(QObject *parent)
    : QObject(parent),
      m_isWorking(false),
      m_noError(false),
      m_errorMessage(QString())
{
}

QmlHelper::~QmlHelper()
{
}

void QmlHelper::checkServer(const QString &username, const QString &password, const QString &path)
{
    m_errorMessage.clear();
    Q_EMIT errorMessageChanged();

    m_username = username;
    m_password = password;

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

    if (url.host().isEmpty()) {
        return;
    }

    checkServer(url);
}

void QmlHelper::checkServer(const QUrl &url)
{
    qDebug() << "Checking for ownCloud instance at" << url;
    setWorking(true);
    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(0);
    connect(job, SIGNAL(data(KIO::Job*,QByteArray)), SLOT(dataReceived(KIO::Job*,QByteArray)));
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(fileChecked(KJob*)));
}

void QmlHelper::figureOutServer(const QUrl& url)
{
    if (/*url == QLatin1String("/") ||*/ url.isEmpty()) {
        serverCheckResult(false);
        return;
    }

    m_json.clear();

    qDebug() << "Received url to figure out:" << url;
    // This needs 2x up cause first it just removes the status.php
    // and only the second call actually moves up
    QUrl urlUp = KIO::upUrl(KIO::upUrl(url));
    urlUp.setPath(urlUp.path() + '/' + "status.php");

    if (urlUp != url) {
        checkServer(urlUp.adjusted(QUrl::NormalizePathSegments));
    } else {
        serverCheckResult(false);
    }
}

void QmlHelper::dataReceived(KIO::Job *job, const QByteArray &data)
{
    Q_UNUSED(job);
    m_json.append(data);
}

void QmlHelper::fileChecked(KJob* job)
{
    KIO::TransferJob *kJob = qobject_cast<KIO::TransferJob *>(job);
    if (kJob->error()) {
        qDebug() << job->errorString();
        qDebug() << job->errorText();
        figureOutServer(kJob->url());
        return;
    }

    QJsonDocument parser = QJsonDocument::fromJson(m_json);
    QJsonObject map = parser.object();
    if (!map.contains("version")) {
        figureOutServer(kJob->url());
        qDebug() << "No json";
        return;
    }

    m_server = kJob->url().adjusted(QUrl::RemoveFilename).toString();
    qDebug() << "ownCloud appears to be running at the specified URL";
    serverCheckResult(true);
}

void QmlHelper::setWorking(bool start)
{
    if (start == m_isWorking) {
        return;
    }

    m_isWorking = start;
    Q_EMIT isWorkingChanged();
}

void QmlHelper::serverCheckResult(bool result)
{
    m_noError = result;
    Q_EMIT noErrorChanged();

    if (!result) {
        m_errorMessage = i18n("Unable to connect to ownCloud at the given server URL. Please check the server URL.");
        setWorking(false);
    } else {
        m_errorMessage.clear();

        qDebug() << "Server URL ok, checking auth...";

        m_json.clear();

        QUrl url(m_server);

        url.setUserName(m_username);
        url.setPassword(m_password);

        url = url.adjusted(QUrl::StripTrailingSlash);
        url.setPath(url.path() + '/' + "remote.php/webdav");
        // Send a basic PROPFIND command to test access
        const QString requestStr = QStringLiteral(
            "<d:propfind xmlns:d=\"DAV:\">"
            "<d:prop>"
            "<d:current-user-principal />"
            "</d:prop>"
            "</d:propfind>");

        KIO::DavJob *job = KIO::davPropFind(url, QDomDocument(requestStr), "1", KIO::HideProgressInfo);
        connect(job, SIGNAL(finished(KJob*)), this, SLOT(authCheckResult(KJob*)));
        connect(job, SIGNAL(data(KIO::Job*,QByteArray)), SLOT(dataReceived(KIO::Job*,QByteArray)));

        QVariantMap metadata{{"cookies","none"}, {"no-cache",true}};

        job->setMetaData(metadata);
        job->setUiDelegate(0);
        job->start();
    }

    Q_EMIT errorMessageChanged();

}

void QmlHelper::authCheckResult(KJob *job)
{
    if (job->error()) {
        qDebug() << job->errorString();
        qDebug() << job->errorText();
    }

    KIO::DavJob *kJob = qobject_cast<KIO::DavJob*>(job);
    qDebug() << "Auth job finished, received error page:" << kJob->isErrorPage();

    if (kJob->isErrorPage()) {
        m_errorMessage = i18n("Unable to authenticate using the provided username and password");
    } else {
        m_errorMessage.clear();
    }

    Q_EMIT errorMessageChanged();

    m_noError = !kJob->isErrorPage();
    Q_EMIT noErrorChanged();
    setWorking(false);
}

bool QmlHelper::isWorking()
{
    return m_isWorking;
}

bool QmlHelper::noError()
{
    return m_noError;
}

QString QmlHelper::errorMessage() const
{
    return m_errorMessage;
}

void QmlHelper::finish(bool contactsEnabled)
{
    QVariantMap data;
    data.insert("server", m_server);

    if (!contactsEnabled) {
        data.insert("__service/owncloud-contacts", false);
    }

    QUrl carddavUrl(m_server);
    carddavUrl.setPath(carddavUrl.path() + QString("/remote.php/carddav/addressbooks/%1").arg(m_username));

    data.insert("carddavUrl", carddavUrl);

    Q_EMIT wizardFinished(m_username, m_password, data);
}

#include "qmlhelper.moc"
