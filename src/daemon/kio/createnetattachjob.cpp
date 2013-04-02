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

#include <QApplication>
#include <QWidget>

#include <Accounts/Manager>

#include <KUrl>
#include <KGlobal>
#include <KDirNotify>
#include <KStandardDirs>
#include <KWallet/Wallet>
#include <KConfig>
#include <KConfigGroup>
#include <KDebug>

using namespace KWallet;

CreateNetAttachJob::CreateNetAttachJob(QObject* parent)
 : KJob(parent)
{

}

void CreateNetAttachJob::start()
{
    QMetaObject::invokeMethod(this, "createNetAttach", Qt::QueuedConnection);
}

void CreateNetAttachJob::createNetAttach()
{
    
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");

    KUrl url(m_host);
    url.setUser(m_username);
    url.setScheme("webdav");
    url.addPath("files/webdav.php/");

    QString path = KGlobal::dirs()->saveLocation("remote_entries");
    path +=  m_name + "ownCloud.desktop";

    qDebug() << "Creating knetAttach place";
    qDebug() << path;
    qDebug() << url.prettyUrl();

    KConfig _desktopFile( path, KConfig::SimpleConfig );
    KConfigGroup desktopFile(&_desktopFile, "Desktop Entry");

    desktopFile.writeEntry("Icon", m_icon);
    desktopFile.writeEntry("Name", "Runners-ID-Storage");
    desktopFile.writeEntry("Type", "Link");
    desktopFile.writeEntry("URL", url.prettyUrl());
    desktopFile.writeEntry("Charset", url.fileEncoding());
    desktopFile.sync();

    org::kde::KDirNotify::emitFilesAdded( "remote:/" );

    QString walletUrl(url.scheme());
    walletUrl.append("-");
    walletUrl.append(m_username);
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

    QString password;
    wallet->setFolder("WebAccounts");
    wallet->readPassword("owncloud-" + m_username, password);

    QMap<QString, QString> info;
    info["login"] = m_username;
    info["password"] = password;

    wallet->setFolder("Passwords");
    wallet->writeMap(walletUrl, info);
    wallet->sync();

//     m_config.group("services").writeEntry("File", 1);
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

QString CreateNetAttachJob::name() const
{
    return m_name;   
}

void CreateNetAttachJob::setName(const QString &name)
{
    m_name = name;
}

QString CreateNetAttachJob::icon() const
{
    return m_icon;
}

void CreateNetAttachJob::setIcon(const QString &icon)
{
    m_icon = icon;
}
