/*
 *  SPDX-FileCopyrightText: 2013 Alejandro Fiestas Olivares <afiestas@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "../src/daemon/kio/createnetattachjob.h"
#include "../src/daemon/kio/removenetattachjob.h"

#include <QTest>

#include <KConfig>
#include <KConfigGroup>
#include <KDirNotify>
#include <KWallet/KWallet>

#include <QDBusAbstractAdaptor>
#include <QDBusConnection>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTimer>

using namespace KWallet;
class testNetAttachJobs : public QObject
{
    Q_OBJECT

public:
    explicit testNetAttachJobs(QObject *parent = 0);
private Q_SLOTS:
    void testCreate();
    void testRemove();

private:
    void enterLoop();

    QTimer m_timer;
    QEventLoop m_eventLoop;
};

testNetAttachJobs::testNetAttachJobs(QObject *parent)
    : QObject(parent)
{
    m_timer.setSingleShot(true);
    m_timer.setInterval(10000); // 10 seconds timeout for eventloop

    connect(&m_timer, &QTimer::timeout, &m_eventLoop, &QEventLoop::quit);
}

void testNetAttachJobs::testCreate()
{
    QString destPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    destPath.append("/remoteview/test-unique-id.desktop");

    qDebug() << destPath;

    org::kde::KDirNotify *watch = new org::kde::KDirNotify(QDBusConnection::sessionBus().baseService(), QString(), QDBusConnection::sessionBus());
    connect(watch, &org::kde::KDirNotify::FilesAdded, &m_eventLoop, &QEventLoop::quit);

    QSignalSpy signalSpy(watch, &org::kde::KDirNotify::FilesAdded);

    CreateNetAttachJob *job = new CreateNetAttachJob(this) job->setHost("host.com");
    job->setUsername("username");
    job->setPassword("password");
    job->setIcon("modem");
    job->setUniqueId("test-unique-id");
    job->setPath("files/webdav.php/");
    job->setName("test-service");
    job->setRealm("testRealm");
    job->exec();

    enterLoop();

    QCOMPARE(signalSpy.count(), 1);
    QCOMPARE(signalSpy.first().first().toString(), QLatin1String("remote:/"));
    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), 0, Wallet::Synchronous);
    wallet->setFolder("Passwords");

    QVERIFY2(QFile::exists(destPath), "Desktop file has not been created");
    QVERIFY2(wallet->hasEntry("webdav-username@host.com:-1-testRealm"), "Wallet realm entry does not exists");
    QVERIFY2(wallet->hasEntry("webdav-username@host.com:-1-webdav"), "Wallet schema entry does not exists");

    KConfig _desktopFile(destPath, KConfig::SimpleConfig);
    KConfigGroup desktopFile(&_desktopFile, "Desktop Entry");
    QCOMPARE(desktopFile.readEntry("Icon", ""), QLatin1String("modem"));
    QCOMPARE(desktopFile.readEntry("Name", ""), QLatin1String("test-service"));
    QCOMPARE(desktopFile.readEntry("Type", ""), QLatin1String("Link"));
    QCOMPARE(desktopFile.readEntry("URL", ""), QLatin1String("webdav://username@host.com/files/webdav.php/"));
    QCOMPARE(desktopFile.readEntry("Charset", ""), QLatin1String(""));

    QMap<QString, QString> data;
    int result = wallet->readMap("webdav-username@host.com:-1-testRealm", data);
    QCOMPARE(result, 0);
    result = wallet->readMap("webdav-username@host.com:-1-webdav", data);
    QCOMPARE(result, 0);

    QVERIFY2(data.keys().contains("login"), "Login data is not stored in the wallet");
    QVERIFY2(data.keys().contains("password"), "Password data is not stored in the wallet");
    QCOMPARE(data["login"], QLatin1String("username"));
    QCOMPARE(data["password"], QLatin1String("password"));
}

void testNetAttachJobs::testRemove()
{
    QString destPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    destPath.append("test-unique-id.desktop");

    org::kde::KDirNotify *watch = new org::kde::KDirNotify(QDBusConnection::sessionBus().baseService(), QString(), QDBusConnection::sessionBus());
    connect(watch, &org::kde::KDirNotify::FilesRemoved, &m_eventLoop, &QEventLoop::quit);

    QSignalSpy signalSpy(watch, &org::kde::KDirNotify::FilesRemoved);

    RemoveNetAttachJob *job = new RemoveNetAttachJob(this);
    job->setUniqueId("test-unique-id");
    job->exec();

    enterLoop();
    QCOMPARE(signalSpy.count(), 1);
    QCOMPARE(signalSpy.first().first().toStringList().first(), QLatin1String("remote:/test-unique-id"));
    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), 0, Wallet::Synchronous);
    wallet->setFolder("Passwords");

    QVERIFY2(!QFile::exists(destPath), "Desktop file has not been removed");
    QVERIFY2(!wallet->hasEntry("webdav-username@host.com:-1-testRealm"), "Wallet realm entry still exists");
    QVERIFY2(!wallet->hasEntry("webdav-username@host.com:-1-webdav"), "Wallet schema entry still exists");
}

void testNetAttachJobs::enterLoop()
{
    m_timer.start();
    m_eventLoop.exec();
}

QTEST_MAIN(testNetAttachJobs)

#include "testnetattachjobs.moc"
