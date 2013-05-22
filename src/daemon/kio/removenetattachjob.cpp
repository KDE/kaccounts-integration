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

#include "removenetattachjob.h"

#include <QtCore/QFile>
#include <QtGui/QWidget>
#include <QtGui/QApplication>

#include <KDebug>
#include <KGlobal>
#include <KStandardDirs>
#include <KWallet/Wallet>
#include <KDirNotify>
#include <KConfig>

using namespace KWallet;
RemoveNetAttachJob::RemoveNetAttachJob(QObject* parent)
 : KJob(parent)
 , m_wallet(0)
{
}

RemoveNetAttachJob::~RemoveNetAttachJob()
{
    delete m_wallet;
}

void RemoveNetAttachJob::start()
{
    QMetaObject::invokeMethod(this, "removeNetAttach", Qt::QueuedConnection);
}

void RemoveNetAttachJob::removeNetAttach()
{
    WId windowId = 0;
    if (qApp->activeWindow()) {
        windowId = qApp->activeWindow()->winId();
    }
    m_wallet = Wallet::openWallet(Wallet::NetworkWallet(), windowId, Wallet::Asynchronous);
    connect(m_wallet, SIGNAL(walletOpened(bool)), SLOT(walletOpened(bool)));
}

void RemoveNetAttachJob::walletOpened(bool opened)
{
    kDebug();
    if (!opened) {
        setError(-1);
        setErrorText("Can't open wallet");
        emitResult();
        return;
    }

    deleteDesktopFile();
}

void RemoveNetAttachJob::deleteDesktopFile()
{
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");
    QString path = KGlobal::dirs()->saveLocation("remote_entries");
    path.append(m_uniqueId + ".desktop");

    KConfig _desktopFile(path, KConfig::SimpleConfig);
    KConfigGroup desktopFile(&_desktopFile, "Desktop Entry");

    KUrl url(desktopFile.readEntry("URL", KUrl()));
    Q_ASSERT(!url.isEmpty());

    kDebug() << url.userName() << url.host() << url;

    QFile::remove(path);
    org::kde::KDirNotify::emitFilesRemoved(QStringList("remote:/" + m_uniqueId));

    QString walletUrl("webdav");
    walletUrl.append("-");
    walletUrl.append(url.userName());
    walletUrl.append("@");
    walletUrl.append(url.host());
    walletUrl.append(":-1");//Overwrite the first option

    m_wallet->setFolder("Passwords");
    QStringList entries = m_wallet->entryList();
    Q_FOREACH(const QString &entry, entries) {
        if (!entry.startsWith(walletUrl)) {
            continue;
        }
        m_wallet->removeEntry(entry);
    }

    emitResult();
}

QString RemoveNetAttachJob::uniqueId() const
{
    return m_uniqueId;
}

void RemoveNetAttachJob::setUniqueId(const QString& uniqueId)
{
    m_uniqueId = uniqueId;
}