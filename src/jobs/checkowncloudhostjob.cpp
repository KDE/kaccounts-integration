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

#include "checkowncloudhostjob.h"

#include <qjson/parser.h>

#include <QDebug>
#include <KIO/Job>
#include <KIO/Global>

CheckOwncloudHostJob::CheckOwncloudHostJob(QObject* parent): KJob(parent)
{

}

CheckOwncloudHostJob::~CheckOwncloudHostJob()
{

}

void CheckOwncloudHostJob::start()
{
    QMetaObject::invokeMethod(this, "requestStatus", Qt::QueuedConnection);
}

void CheckOwncloudHostJob::setUrl(const QString& url)
{
    m_url = url;
    if (m_url.startsWith("http://") || m_url.startsWith("https://")) {
        return;
    }

    m_url.prepend("http://");
}

QString CheckOwncloudHostJob::url() const
{
    return m_url;
}

void CheckOwncloudHostJob::requestStatus()
{
    QUrl url(m_url);

    url.setPath("status.php");

    qDebug() << url;

    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    job->setUiDelegate(0);

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)), SLOT(dataReceived(KIO::Job*,QByteArray)));
    connect(job, SIGNAL(finished(KJob*)), SLOT(fileDownloaded(KJob*)));
}

void CheckOwncloudHostJob::dataReceived(KIO::Job* job, const QByteArray& data)
{
    m_json.append(data);
}

void CheckOwncloudHostJob::fileDownloaded(KJob* job)
{
    KIO::TransferJob *kJob = qobject_cast<KIO::TransferJob *>(job);
    if (kJob->error()) {
        qDebug() << job->errorString();
        qDebug() << job->errorText();
        figureOutServer(kJob->url());
        return;
    }

    QJson::Parser parser;
    QMap <QString, QVariant> map = parser.parse(m_json).toMap();
    if (!map.contains("version")) {
        figureOutServer(kJob->url());
        return;
    }

    emitResult();
}

void CheckOwncloudHostJob::figureOutServer(const QUrl &url)
{
    if (url.isEmpty() || url.path() == "/") {
        setError(-1);
        setErrorText("Unable to find the host");
        emitResult();
        return;
    }

    QUrl upUrl = KIO::upUrl(url);
    //FIXME?
//     upUrl.setFileName("");
    m_url = upUrl.toString();

    m_json.clear();

    requestStatus();
}

