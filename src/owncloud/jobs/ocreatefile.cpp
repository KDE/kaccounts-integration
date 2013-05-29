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

#include "ocreatefile.h"

#include <QWidget>
#include <QApplication>
#include <QtGui/QDesktopServices>

#include <KGlobal>
#include <KConfig>
#include <KDirNotify>
#include <KStandardDirs>
#include <KWallet/Wallet>
#include <kio/job.h>
#include <KDebug>

using namespace KWallet;

OCreateFile::OCreateFile(KConfigGroup group, QObject* parent)
 : KJob(parent)
 , m_config(group)
{

}

OCreateFile::~OCreateFile()
{

}

void OCreateFile::start()
{
    QMetaObject::invokeMethod(this, "getRealm", Qt::QueuedConnection);
}

void OCreateFile::getRealm()
{
    kDebug();
    KUrl url(m_config.readEntry("server", ""));
    url.setUser(m_config.name());
    url.setScheme("webdav");
    url.addPath("files/webdav.php/");

    kDebug() << url;
    KIO::TransferJob* job = KIO::get(url , KIO::NoReload, KIO::HideProgressInfo);
    connect(job, SIGNAL(finished(KJob*)), SLOT(gotRealm(KJob*)));
    KIO::MetaData data;
    data.insert("PropagateHttpHeader", "true");
    job->setMetaData(data);
    job->setUiDelegate(0);
    job->start();
}

void OCreateFile::gotRealm(KJob* job)
{
    KIO::TransferJob* hJob = qobject_cast<KIO::TransferJob*>(job);
    QRegExp rx("www-authenticate: Basic realm=\"(\\S+)\"\n");
    QString headers = hJob->metaData().value("HTTP-Headers");
    if (rx.indexIn(headers) != -1) {
        m_realm = rx.cap(1);
    }

    createNetAttach();
}

void OCreateFile::createNetAttach()
{
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");

    KUrl url(m_config.readEntry("server", ""));
    url.setUser(m_config.name());
    url.setScheme("webdav");
    url.addPath("files/webdav.php/");

    QString path = KGlobal::dirs()->saveLocation("remote_entries");
    path +=  m_config.name() + "ownCloud.desktop";

    kDebug() << "Creating knetAttach place";
    kDebug() << path;
    kDebug() << url.prettyUrl();

    m_config.group("private").writeEntry("fileDesktop", path);

    KConfig _desktopFile( path, KConfig::SimpleConfig );
    KConfigGroup desktopFile(&_desktopFile, "Desktop Entry");

    if (m_config.readEntry("type", "") == "runnerid") {
        desktopFile.writeEntry("Icon", "folder-remote");
        desktopFile.writeEntry("Name", "Runners-ID-Storage");
    } else {
        desktopFile.writeEntry("Icon", "folder-remote");
        desktopFile.writeEntry("Name", url.host());
    }
    desktopFile.writeEntry("Type", "Link");
    desktopFile.writeEntry("URL", url.prettyUrl());
    desktopFile.writeEntry("Charset", url.fileEncoding());
    desktopFile.sync();
    _desktopFile.sync();

    org::kde::KDirNotify::emitFilesAdded( "remote:/" );

    if (m_config.readEntry("type", "") == "runnerid") {
        QString networkPath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
        networkPath.append("/Network/");
        if (QDir().exists(networkPath)) {
            QDir().mkdir(networkPath);
        }
        networkPath.append("Runners-ID-Storage");
        qDebug() << path << networkPath;
        qDebug() << QFile::copy(path, networkPath);
    }

    QString walletUrl(url.scheme());
    walletUrl.append("-");
    walletUrl.append(m_config.name());
    walletUrl.append("@");
    walletUrl.append(url.host());
    walletUrl.append(":-1-");//Overwrite the first option

    WId windowId = 0;
    if (qApp->activeWindow()) {
        windowId = qApp->activeWindow()->winId();
    }
    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), windowId, Wallet::Synchronous);

    if (!wallet->isOpen() || !wallet->isEnabled()) {
        return;
    }

    QString password;
    wallet->setFolder("WebAccounts");
    if (m_config.readEntry("type", "") == "runnerid") {
        wallet->readPassword("runnerid-" + m_config.name(), password);
    } else {
        wallet->readPassword("owncloud-" + m_config.name(), password);
    }

    QMap<QString, QString> info;
    info["login"] = m_config.name();
    info["password"] = password;

    wallet->setFolder("Passwords");
    if (!m_realm.isEmpty()) {
        wallet->writeMap(walletUrl + m_realm, info);
    }
    wallet->writeMap(walletUrl + "webdav", info);
    wallet->sync();

    m_config.group("services").writeEntry("File", 1);
    emitResult();
}