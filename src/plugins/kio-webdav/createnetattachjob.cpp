/*
 *  SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "createnetattachjob.h"

#include <Accounts/Manager>

#include <KConfig>
#include <KConfigGroup>
#include <KDirNotify>
#include <KIO/Job>
#include <KWallet>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QRegularExpression>
#include <QUrl>
#include <QWidget>

using namespace KWallet;

CreateNetAttachJob::CreateNetAttachJob(QObject *parent)
    : KJob(parent)
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
        setErrorText(QStringLiteral("Can't open wallet"));
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
    url.setScheme(QStringLiteral("webdavs"));
    url.setPath(m_path);

    if (!m_realm.isEmpty()) {
        createDesktopFile(url);
        return;
    }

    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    connect(job, &KIO::TransferJob::finished, this, &CreateNetAttachJob::gotRealm);
    KIO::MetaData data;
    data.insert(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    job->setMetaData(data);
    job->setUiDelegate(nullptr);
    job->start();
}

void CreateNetAttachJob::gotRealm(KJob *job)
{
    KIO::TransferJob *hJob = qobject_cast<KIO::TransferJob *>(job);
    QRegularExpression rx(QStringLiteral("www-authenticate: Basic realm=\"([^\"]+)\""));
    Q_ASSERT(rx.isValid());
    QString headers = hJob->metaData().value(QStringLiteral("HTTP-Headers"));

    auto match = rx.match(headers);

    if (match.hasMatch()) {
        m_realm = match.captured(1);
    }

    createDesktopFile(hJob->url());
}

void CreateNetAttachJob::createDesktopFile(const QUrl &url)
{
    qDebug();

    const QString folderPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/remoteview/");

    QDir saveDir(folderPath);
    if (!saveDir.exists()) {
        if (!saveDir.mkpath(folderPath)) {
            qWarning() << "Directory" << folderPath << "for storage couldn't be created!";
        }
    }
    const QString path = folderPath + m_uniqueId + QStringLiteral(".desktop");

    qDebug() << "Creating knetAttach place";
    qDebug() << path;
    qDebug() << url.host();
    qDebug() << url.toString();

    KConfig _desktopFile(path, KConfig::SimpleConfig);
    KConfigGroup desktopFile(&_desktopFile, "Desktop Entry");

    desktopFile.writeEntry("Icon", m_icon);
    desktopFile.writeEntry("Name", m_name);
    desktopFile.writeEntry("Type", "Link");
    desktopFile.writeEntry("URL", url.toString());
    //     desktopFile.writeEntry("Charset", url.fileEncoding());
    desktopFile.sync();

    QString walletUrl(url.scheme());
    walletUrl.append(QStringLiteral("-"));
    walletUrl.append(m_username);
    walletUrl.append(QStringLiteral("@"));
    walletUrl.append(url.host());
    walletUrl.append(QStringLiteral(":-1-")); // Overwrite the first option

    QMap<QString, QString> info;
    info[QStringLiteral("login")] = m_username;
    info[QStringLiteral("password")] = m_password;

    m_wallet->setFolder(QStringLiteral("Passwords"));

    if (!m_realm.isEmpty()) {
        m_wallet->writeMap(walletUrl + m_realm, info);
    }
    m_wallet->writeMap(walletUrl + QStringLiteral("webdav"), info);
    m_wallet->sync();

    org::kde::KDirNotify::emitFilesAdded(QUrl(QStringLiteral("remote:/")));

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

void CreateNetAttachJob::setPath(const QString &path)
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

void CreateNetAttachJob::setName(const QString &name)
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

void CreateNetAttachJob::setUniqueId(const QString &uniqueId)
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
