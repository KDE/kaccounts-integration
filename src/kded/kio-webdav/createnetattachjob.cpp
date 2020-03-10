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

#include "createnetattachjob.h"

#include <Accounts/Manager>

#include <KDirNotify>
#include <KWallet/KWallet>
#include <KConfig>
#include <KIO/Job>
#include <KConfigGroup>

#include <QApplication>
#include <QWidget>
#include <QUrl>
#include <QDir>
#include <QDebug>

using namespace KWallet;

CreateNetAttachJob::CreateNetAttachJob(QObject *parent)
 : KJob(parent)
 , m_wallet(0)
{

}

CreateNetAttachJob::~CreateNetAttachJob()
{
    delete m_wallet;
}

void CreateNetAttachJob::start()
{
    QMetaObject::invokeMethod(this, "createNetAttach", Qt::QueuedConnection);
}

void CreateNetAttachJob::createNetAttach()
{
    qDebug();
    WId windowId = 0;
    if (qApp->activeWindow()) {
        windowId = qApp->activeWindow()->winId();
    }
    m_wallet = Wallet::openWallet(Wallet::NetworkWallet(), windowId, Wallet::Asynchronous);

    connect(m_wallet, &KWallet::Wallet::walletOpened, this, &CreateNetAttachJob::walletOpened);
}

void CreateNetAttachJob::walletOpened(bool opened)
{
    qDebug();
    if (!opened) {
        setError(-1);
        setErrorText("Can't open wallet");
        emitResult();
        return;
    }

    getRealm();
}

void CreateNetAttachJob::getRealm()
{
    qDebug();
    QUrl url;
    url.setHost(m_host);
    url.setUserName(m_username);
    url.setScheme("webdav");
    url = url.adjusted(QUrl::StripTrailingSlash);
    url.setPath(url.path() + '/' + m_path);

    if (!m_realm.isEmpty()) {
        createDesktopFile(url);
        return;
    }

    KIO::TransferJob *job = KIO::get(url , KIO::NoReload, KIO::HideProgressInfo);
    connect(job, &KIO::TransferJob::finished, this, &CreateNetAttachJob::gotRealm);
    KIO::MetaData data;
    data.insert("PropagateHttpHeader", "true");
    job->setMetaData(data);
    job->setUiDelegate(0);
    job->start();
}

void CreateNetAttachJob::gotRealm(KJob *job)
{
    KIO::TransferJob *hJob = qobject_cast<KIO::TransferJob*>(job);
    QRegExp rx("www-authenticate: Basic realm=\"(\\S+)\"\n");
    QString headers = hJob->metaData().value("HTTP-Headers");
    if (rx.indexIn(headers) != -1) {
        m_realm = rx.cap(1);
    }

    createDesktopFile(hJob->url());
}


void CreateNetAttachJob::createDesktopFile(const QUrl &url)
{
    qDebug();

    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    path.append(QStringLiteral("/remoteview/"));

    QDir saveDir(path);
    if (!saveDir.exists()) {
        if (!saveDir.mkpath(path)) {
            qWarning() << "Directory" << path << "for storage couldn't be created!";
        }
    }
    path += m_uniqueId + ".desktop";

    qDebug() << "Creating knetAttach place";
    qDebug() << path;
    qDebug() << url.host();
    qDebug() << url.toString();

    KConfig _desktopFile( path, KConfig::SimpleConfig );
    KConfigGroup desktopFile(&_desktopFile, "Desktop Entry");

    desktopFile.writeEntry("Icon", m_icon);
    desktopFile.writeEntry("Name", m_name);
    desktopFile.writeEntry("Type", "Link");
    desktopFile.writeEntry("URL", url.toString());
//     desktopFile.writeEntry("Charset", url.fileEncoding());
    desktopFile.sync();

    QString walletUrl(url.scheme());
    walletUrl.append("-");
    walletUrl.append(m_username);
    walletUrl.append("@");
    walletUrl.append(url.host());
    walletUrl.append(":-1-");//Overwrite the first option

    QMap<QString, QString> info;
    info["login"] = m_username;
    info["password"] = m_password;

    m_wallet->setFolder("Passwords");

    if (!m_realm.isEmpty()) {
        m_wallet->writeMap(walletUrl + m_realm, info);
    }
    m_wallet->writeMap(walletUrl + "webdav", info);
    m_wallet->sync();

    org::kde::KDirNotify::emitFilesAdded(QUrl("remote:/"));

    emitResult();
}

QString CreateNetAttachJob::host() const
{
    return m_host;
}
void CreateNetAttachJob::setHost(const QString &host)
{
    m_host = host;
}

QString CreateNetAttachJob::path() const
{
    return m_path;
}

void CreateNetAttachJob::setPath(const QString& path)
{
    m_path = path;
}

QString CreateNetAttachJob::realm() const
{
    return m_realm;
}

void CreateNetAttachJob::setRealm(const QString &realm)
{
    m_realm = realm;
}

QString CreateNetAttachJob::name() const
{
    return m_name;
}

void CreateNetAttachJob::setName(const QString& name)
{
    m_name = name;
}

QString CreateNetAttachJob::username() const
{
    return m_username;
}

void CreateNetAttachJob::setUsername(const QString &username)
{
    m_username = username;
}

QString CreateNetAttachJob::password() const
{
    return m_password;
}

void CreateNetAttachJob::setPassword(const QString &password)
{
    m_password = password;
}

QString CreateNetAttachJob::uniqueId() const
{
    return m_uniqueId;
}

void CreateNetAttachJob::setUniqueId(const QString& uniqueId)
{
    m_uniqueId = uniqueId;
}

QString CreateNetAttachJob::icon() const
{
    return m_icon;
}

void CreateNetAttachJob::setIcon(const QString &icon)
{
    m_icon = icon;
}
