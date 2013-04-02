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
    WId windowId = 0;
    if (qApp->activeWindow()) {
        windowId = qApp->activeWindow()->winId();
    }
    m_wallet = Wallet::openWallet(Wallet::NetworkWallet(), windowId, Wallet::Asynchronous);
    connect(m_wallet, SIGNAL(walletOpened(bool)), SLOT(walletOpened(bool)));
}

void CreateNetAttachJob::walletOpened(bool opened)
{
    kDebug();
    if (!opened) {
        setError(-1);
        setErrorText("Can't open wallet");
        emitResult();
        return;
    }

    createDesktopFile();
}

void CreateNetAttachJob::createDesktopFile()
{
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");

    KUrl url;
    url.setHost(m_host);
    url.setUser(m_username);
    url.setScheme("webdav");
    url.addPath("files/webdav.php/");

    QString path = KGlobal::dirs()->saveLocation("remote_entries");
    path +=  m_username + "_"  + m_host + ".desktop";

    qDebug() << "Creating knetAttach place";
    qDebug() << path;
    qDebug() << url.host();
    qDebug() << url.prettyUrl();

    KConfig _desktopFile( path, KConfig::SimpleConfig );
    KConfigGroup desktopFile(&_desktopFile, "Desktop Entry");

    desktopFile.writeEntry("Icon", m_icon);
    desktopFile.writeEntry("Name", "Runners-ID-Storage");
    desktopFile.writeEntry("Type", "Link");
    desktopFile.writeEntry("URL", url.prettyUrl());
    desktopFile.writeEntry("Charset", url.fileEncoding());
    desktopFile.sync();

    QString walletUrl(url.scheme());
    walletUrl.append("-");
    walletUrl.append(m_username);
    walletUrl.append("@");
    walletUrl.append(url.host());
    walletUrl.append(":-1");//Overwrite the first option

    QMap<QString, QString> info;
    info["login"] = m_username;
    info["password"] = m_password;

    m_wallet->setFolder("Passwords");
    m_wallet->writeMap(walletUrl, info);
    m_wallet->sync();

    org::kde::KDirNotify::emitFilesAdded( "remote:/" );

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

QString CreateNetAttachJob::icon() const
{
    return m_icon;
}

void CreateNetAttachJob::setIcon(const QString &icon)
{
    m_icon = icon;
}
