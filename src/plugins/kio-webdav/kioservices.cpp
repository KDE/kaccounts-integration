/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kioservices.h"
#include "core.h"
#include "getcredentialsjob.h"

#include <QApplication>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QStandardPaths>
#include <QWidget>

#include <KConfig>
#include <KConfigGroup>
#include <KDirNotify>
#include <KIO/TransferJob>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KWallet>

#include <Accounts/Manager>

using namespace KWallet;

K_PLUGIN_CLASS_WITH_JSON(KIOServices, "kio-webdav.json")

KIOServices::KIOServices(QObject *parent, const QVariantList &args)
    : KAccounts::KAccountsDPlugin(parent, args)
{
}

KIOServices::~KIOServices() = default;

void KIOServices::onAccountCreated(const Accounts::AccountId accId, const Accounts::ServiceList &serviceList)
{
    qDebug();
    for (const Accounts::Service &service : serviceList) {
        if (service.serviceType() != QLatin1String("dav-storage")) {
            qDebug() << "Ignoring: " << service.serviceType();
            continue;
        }
        if (isEnabled(accId, service.name())) {
            qDebug() << "Already configured: " << service.name();
            continue;
        }

        qDebug() << "Creating: " << service.name() << "Of type: " << service.serviceType();
        enableService(accId, service);
    }
}

void KIOServices::onAccountRemoved(const Accounts::AccountId accId)
{
    qDebug();
    const QString accountId = QString::number(accId) + QStringLiteral("_");

    const QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/remoteview/");

    QDirIterator i(path, QDir::NoDotAndDotDot | QDir::Files);
    while (i.hasNext()) {
        i.next();
        if (!i.fileName().startsWith(accountId)) {
            continue;
        }

        QString serviceName = i.fileName();
        qDebug() << "Removing: " << serviceName;
        serviceName = serviceName.mid(accountId.count(), serviceName.indexOf(QLatin1String(".desktop")) - accountId.count());
        qDebug() << "Removing N: " << serviceName;
        disableService(accId, serviceName);
    }
}

void KIOServices::onServiceEnabled(const Accounts::AccountId accId, const Accounts::Service &service)
{
    if (service.serviceType() != QLatin1String("dav-storage")) {
        qDebug() << "Ignoring: " << service.serviceType();
        return;
    }
    if (isEnabled(accId, service.name())) {
        qDebug() << "Already configured: " << service.name();
        return;
    }

    enableService(accId, service);
}

void KIOServices::onServiceDisabled(const Accounts::AccountId accId, const Accounts::Service &service)
{
    if (service.serviceType() != QLatin1String("dav-storage")) {
        qDebug() << "Ignoring: " << service.serviceType();
        return;
    }
    if (!isEnabled(accId, service.name())) {
        qDebug() << "Already not configured: " << service.name();
        return;
    }

    disableService(accId, service.name());
}

void KIOServices::enableService(const Accounts::AccountId accId, const Accounts::Service &service)
{
    createNetAttach(accId, service);
}

void KIOServices::disableService(const Accounts::AccountId accId, const QString &serviceName)
{
    removeNetAttach(QString::number(accId) + QStringLiteral("_") + serviceName);
}

bool KIOServices::isEnabled(const Accounts::AccountId accId, const QString &serviceName)
{
    const QString uniqueId(QString::number(accId) + QStringLiteral("_") + serviceName);

    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    path += QStringLiteral("/remoteview/") + uniqueId + QStringLiteral(".desktop");

    return QFile::exists(path);
}

QCoro::Task<void> KIOServices::createNetAttach(const Accounts::AccountId accountId, const Accounts::Service &_service)
{
    std::unique_ptr<Accounts::Account> account(KAccounts::accountsManager()->account(accountId));
    const Accounts::Service service = _service;

    KAccounts::GetCredentialsJob *job = new KAccounts::GetCredentialsJob(accountId, QString(), QString(), this);
    job->setServiceType(service.serviceType());
    job->start();

    co_await qCoro(job, &KJob::finished);

    if (job->error()) {
        co_return;
    }

    const QVariantMap credentials = job->credentialsData();

    const QString host = account->value(QStringLiteral("dav/host")).toString();
    const QString path = account->value(QStringLiteral("dav/storagePath")).toString();

    account->selectService(service);
    const QString username = credentials[QStringLiteral("UserName")].toString();

    WId windowId = 0;
    if (qApp->activeWindow()) {
        windowId = qApp->activeWindow()->winId();
    }

    auto wallet = Wallet::openWallet(Wallet::NetworkWallet(), windowId, Wallet::Asynchronous);

    bool opened = co_await qCoro(wallet, &KWallet::Wallet::walletOpened);

    if (!opened) {
        co_return;
    }

    QUrl url;
    url.setHost(host);
    url.setUserName(username);
    url.setScheme(QStringLiteral("webdavs"));
    url.setPath(path);

    const QString realm = co_await getRealm(url);

    const QString folderPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/remoteview/");

    QDir saveDir(folderPath);
    if (!saveDir.exists()) {
        if (!saveDir.mkpath(folderPath)) {
            qWarning() << "Directory" << folderPath << "for storage couldn't be created!";
        }
    }
    const QString desktopFilePath = folderPath + QString::number(accountId) + QLatin1Char('_') + service.name() + QStringLiteral(".desktop");

    qDebug() << "Creating knetAttach place";
    qDebug() << desktopFilePath;
    qDebug() << url.host();
    qDebug() << url.toString();

    KConfig _desktopFile(desktopFilePath, KConfig::SimpleConfig);
    KConfigGroup desktopFile(&_desktopFile, QStringLiteral("Desktop Entry"));

    const QString label = KAccounts::accountsManager()->provider(service.provider()).displayName() + QLatin1Char(' ') + service.displayName();

    desktopFile.writeEntry("Icon", service.iconName());
    desktopFile.writeEntry("Name", label);
    desktopFile.writeEntry("Type", "Link");
    desktopFile.writeEntry("URL", url.toString());
    //     desktopFile.writeEntry("Charset", url.fileEncoding());
    desktopFile.sync();

    QString walletUrl(url.scheme());
    walletUrl.append(QStringLiteral("-"));
    walletUrl.append(username);
    walletUrl.append(QStringLiteral("@"));
    walletUrl.append(url.host());
    walletUrl.append(QStringLiteral(":-1-")); // Overwrite the first option

    QMap<QString, QString> info;
    info[QStringLiteral("login")] = username;
    info[QStringLiteral("password")] = credentials[QStringLiteral("Secret")].toString();

    wallet->setFolder(QStringLiteral("Passwords"));

    if (!realm.isEmpty()) {
        wallet->writeMap(walletUrl + realm, info);
    }
    wallet->writeMap(walletUrl + QStringLiteral("webdav"), info);
    wallet->sync();

    org::kde::KDirNotify::emitFilesAdded(QUrl(QStringLiteral("remote:/")));
}

QCoro::Task<QString> KIOServices::getRealm(const QUrl &url)
{
    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    KIO::MetaData data;
    data.insert(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    job->setMetaData(data);
    job->setUiDelegate(nullptr);
    job->start();

    co_await qCoro(job, &KJob::finished);

    QRegularExpression rx(QStringLiteral("www-authenticate: Basic realm=\"([^\"]+)\""));
    Q_ASSERT(rx.isValid());
    QString headers = job->metaData().value(QStringLiteral("HTTP-Headers"));

    auto match = rx.match(headers);

    if (match.hasMatch()) {
        co_return match.captured(1);
    }

    co_return QString();
}

QCoro::Task<void> KIOServices::removeNetAttach(const QString &_id)
{
    QString id = _id;

    WId windowId = 0;
    if (qApp->activeWindow()) {
        windowId = qApp->activeWindow()->winId();
    }
    std::unique_ptr<KWallet::Wallet> wallet(Wallet::openWallet(Wallet::NetworkWallet(), windowId, Wallet::Asynchronous));

    bool opened = co_await qCoro(wallet.get(), &KWallet::Wallet::walletOpened);

    if (!opened) {
        co_return;
    }

    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    path.append(QStringLiteral("/remoteview/") + id + QStringLiteral(".desktop"));

    KConfig _desktopFile(path, KConfig::SimpleConfig);
    KConfigGroup desktopFile(&_desktopFile, QStringLiteral("Desktop Entry"));

    const QUrl url(desktopFile.readEntry("URL", QUrl()));
    Q_ASSERT(!url.isEmpty());

    qDebug() << url.userName() << url.host() << url;

    QFile::remove(path);
    org::kde::KDirNotify::emitFilesRemoved(QList<QUrl>() << QUrl(QStringLiteral("remote:/") + id));

    QString walletUrl(QStringLiteral("webdav"));
    walletUrl.append(QStringLiteral("-"));
    walletUrl.append(url.userName());
    walletUrl.append(QStringLiteral("@"));
    walletUrl.append(url.host());
    walletUrl.append(QStringLiteral(":-1")); // Overwrite the first option

    wallet->setFolder(QStringLiteral("Passwords"));
    const QStringList entries = wallet->entryList();
    for (const QString &entry : entries) {
        if (!entry.startsWith(walletUrl)) {
            continue;
        }
        wallet->removeEntry(entry);
    }
}

#include "kioservices.moc"
