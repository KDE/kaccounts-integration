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

#include <qtest_kde.h>

#include <KGlobal>
#include <KStandardDirs>
#include <KWallet/Wallet>
#include <KDirWatch>
#include <KConfigGroup>
#include <QDBusConnection>
#include <QDBusAbstractAdaptor>

using namespace KWallet;
class testCreateNetAttachJob : public QObject
{
    Q_OBJECT

    private Q_SLOTS:
        void testCreate();
};


void testCreateNetAttachJob::testCreate()
{
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");
    QString destPath = KGlobal::dirs()->saveLocation("remote_entries");
    destPath.append("test-unique-id.desktop");

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

    Wallet *wallet = Wallet::openWallet(Wallet::NetworkWallet(), 0, Wallet::Synchronous);
    wallet->setFolder("Passwords");

    QVERIFY2(QFile::exists(destPath), "Desktop file has not been created");
    QVERIFY2(wallet->hasEntry("webdav-username@host.com:-1-testRealm"), "Wallet entry does not exists");

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

    QVERIFY2(data.keys().contains("login"), "Login data is not stored in the wallet");
    QVERIFY2(data.keys().contains("password"), "Password data is not stored in the wallet");
    QCOMPARE(data["login"], QLatin1String("username"));
    QCOMPARE(data["password"], QLatin1String("password"));

}

QTEST_KDEMAIN_CORE(testCreateNetAttachJob)

#include "testcreatenetattachjob.moc"
