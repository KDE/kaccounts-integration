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

#include <QWidget>
#include <QApplication>

#include <Accounts/Manager>

#include <KGlobal>
#include <KDirNotify>
#include <KStandardDirs>
#include <KWallet/Wallet>
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

void CreateNetAttachJob::setServiceName(const QString& serviceName)
{
    m_serviceName = serviceName;
}

void CreateNetAttachJob::setAccountId(const Accounts::AccountId& accId)
{
    m_accountId = accId;
}

void CreateNetAttachJob::createNetAttach()
{
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");

    Accounts::Manager *manager = new Accounts::Manager();
    Accounts::Account* account = manager->account(m_accountId);

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
        desktopFile.writeEntry("Icon", "netrunnerid");
        desktopFile.writeEntry("Name", "Runners-ID-Storage");
    } else {
        desktopFile.writeEntry("Icon", "owncloud");
        desktopFile.writeEntry("Name", url.host());
    }
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

    QString password;
    wallet->setFolder("WebAccounts");
    wallet->readPassword("owncloud-" + m_config.name(), password);

    QMap<QString, QString> info;
    info["login"] = m_config.name();
    info["password"] = password;

    wallet->setFolder("Passwords");
    wallet->writeMap(walletUrl, info);
    wallet->sync();

    m_config.group("services").writeEntry("File", 1);
    emitResult();
}
