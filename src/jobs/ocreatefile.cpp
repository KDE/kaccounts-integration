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

#include <KGlobal>
#include <KConfig>
#include <KDirNotify>
#include <KStandardDirs>
#include <KWallet/Wallet>
#include <QDebug>

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
    QMetaObject::invokeMethod(this, "createNetAttach", Qt::QueuedConnection);
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

    qDebug() << "Creating knetAttach place";
    qDebug() << path;
    qDebug() << url.prettyUrl();

    KConfig _desktopFile( path, KConfig::SimpleConfig );
    KConfigGroup desktopFile(&_desktopFile, "Desktop Entry");

    desktopFile.writeEntry("Icon", "owncloud");
    desktopFile.writeEntry("Name", url.host());
    desktopFile.writeEntry("Type", "Link");
    desktopFile.writeEntry("URL", url.prettyUrl());
    desktopFile.writeEntry("Charset", url.fileEncoding());
    desktopFile.sync();

    org::kde::KDirNotify::emitFilesAdded( "remote:/" );

    QString walletUrl(url.scheme());
    walletUrl.append("-");
    walletUrl.append(m_config.name());
    walletUrl.append("@");
    walletUrl.append(url.host());
    walletUrl.append(":-1");//Overwrite the first option

    WId windowId = 0;
    if (qApp->activeWindow()) {
        windowId = qApp->activeWindow()->winId();
    }
    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), windowId, Wallet::Synchronous);

    if (!wallet->isOpen() || !wallet->isEnabled()) {
        return;
    }

    QMap<QString, QString> info;
    info["login"] = m_config.name();
    info["password"] = "12344321";
    wallet->setFolder("Passwords");
    wallet->writeMap(walletUrl, info);
    wallet->sync();

    m_config.group("services").writeEntry("File", 1);
    emitResult();
}
