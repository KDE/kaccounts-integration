/*
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "removenetattachjob.h"

#include <QFile>
#include <QWidget>
#include <QApplication>
#include <QUrl>
#include <QDebug>

#include <KWallet/KWallet>
#include <KDirNotify>
#include <KConfig>
#include <KLocalizedString>

using namespace KWallet;
RemoveNetAttachJob::RemoveNetAttachJob(QObject *parent)
 : KJob(parent)
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
    connect(m_wallet, &KWallet::Wallet::walletOpened, this, &RemoveNetAttachJob::walletOpened);
}

void RemoveNetAttachJob::walletOpened(bool opened)
{
    qDebug();
    if (!opened) {
        setError(-1);
        setErrorText(i18n("Can't open wallet"));
        emitResult();
        return;
    }

    deleteDesktopFile();
}

void RemoveNetAttachJob::deleteDesktopFile()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    path.append(QStringLiteral("/remoteview/") + m_uniqueId + QStringLiteral(".desktop"));

    KConfig _desktopFile(path, KConfig::SimpleConfig);
    KConfigGroup desktopFile(&_desktopFile, "Desktop Entry");

    QUrl url(desktopFile.readEntry("URL", QUrl()));
    Q_ASSERT(!url.isEmpty());

    qDebug() << url.userName() << url.host() << url;

    QFile::remove(path);
    org::kde::KDirNotify::emitFilesRemoved(QList<QUrl>() << QUrl(QStringLiteral("remote:/") + m_uniqueId));

    QString walletUrl(QStringLiteral("webdav"));
    walletUrl.append(QStringLiteral("-"));
    walletUrl.append(url.userName());
    walletUrl.append(QStringLiteral("@"));
    walletUrl.append(url.host());
    walletUrl.append(QStringLiteral(":-1"));//Overwrite the first option

    m_wallet->setFolder(QStringLiteral("Passwords"));
    const QStringList entries = m_wallet->entryList();
    for (const QString &entry : entries) {
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

void RemoveNetAttachJob::setUniqueId(const QString &uniqueId)
{
    m_uniqueId = uniqueId;
}
