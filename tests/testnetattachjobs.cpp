/*************************************************************************************
 *  Copyright (C) 2013 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#include "../src/daemon/kio/createnetattachjob.h"
#include "../src/daemon/kio/removenetattachjob.h"

#include <qtest_kde.h>

#include <KGlobal>
#include <KStandardDirs>
#include <KWallet/Wallet>
#include <KDirNotify>
#include <KConfigGroup>
#include <QDBusConnection>
#include <QDBusAbstractAdaptor>

using namespace KWallet;
class testCreateNetAttachJob : public QObject
{
    Q_OBJECT

    public:
        explicit testCreateNetAttachJob(QObject* parent = 0);
    private Q_SLOTS:
        void testCreate();
        void testRemove();
    private:
        void enterLoop();

        QTimer m_timer;
        QEventLoop m_eventLoop;
};

testCreateNetAttachJob::testCreateNetAttachJob(QObject* parent): QObject(parent)
{
    m_timer.setSingleShot(true);
    m_timer.setInterval(10000);// 10 seconds timeout for eventloop

    connect(&m_timer, SIGNAL(timeout()), &m_eventLoop, SLOT(quit()));
}

void testCreateNetAttachJob::testCreate()
{
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");
    QString destPath = KGlobal::dirs()->saveLocation("remote_entries");
    destPath.append("test-unique-id.desktop");

    org::kde::KDirNotify *watch = new org::kde::KDirNotify(
    QDBusConnection::sessionBus().baseService(), QString(), QDBusConnection::sessionBus());
    connect(watch, SIGNAL(FilesAdded(QString)), &m_eventLoop, SLOT(quit()));

    QSignalSpy signalSpy(watch, SIGNAL(FilesAdded(QString)));

    CreateNetAttachJob *job = new CreateNetAttachJob(this);
    job->setHost("host.com");
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

    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), 0, Wallet::Synchronous);
    wallet->setFolder("Passwords");

    QVERIFY2(QFile::exists(destPath), "Desktop file has not been created");
    QVERIFY2(wallet->hasEntry("webdav-username@host.com:-1-testRealm"), "Wallet realm entry does not exists");
    QVERIFY2(wallet->hasEntry("webdav-username@host.com:-1-webdav"), "Wallet schema entry does not exists");

    KConfig _desktopFile(destPath, KConfig::SimpleConfig );
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

void testCreateNetAttachJob::testRemove()
{
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");
    QString destPath = KGlobal::dirs()->saveLocation("remote_entries");
    destPath.append("test-unique-id.desktop");

    org::kde::KDirNotify *watch = new org::kde::KDirNotify(
    QDBusConnection::sessionBus().baseService(), QString(), QDBusConnection::sessionBus());
    connect(watch, SIGNAL(FilesRemoved(QStringList)), &m_eventLoop, SLOT(quit()));

    QSignalSpy signalSpy(watch, SIGNAL(FilesRemoved(QStringList)));

    RemoveNetAttachJob *job = new RemoveNetAttachJob(this);
    job->setHost("host.com");
    job->setUsername("username");
    job->setUniqueId("test-unique-id");
    job->exec();

    enterLoop();
    QCOMPARE(signalSpy.count(), 1);
    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), 0, Wallet::Synchronous);
    wallet->setFolder("Passwords");

    QVERIFY2(!QFile::exists(destPath), "Desktop file has not been removed");
    QVERIFY2(!wallet->hasEntry("webdav-username@host.com:-1-testRealm"), "Wallet realm entry still exists");
    QVERIFY2(!wallet->hasEntry("webdav-username@host.com:-1-webdav"), "Wallet schema entry still exists");
}

void testCreateNetAttachJob::enterLoop()
{
    m_timer.start();
    m_eventLoop.exec();
}

QTEST_KDEMAIN_CORE(testCreateNetAttachJob)

#include "testcreatenetattachjob.moc"
