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

#include "../src/daemon/kio/removenetattachjob.h"

#include <qtest_kde.h>

#include <KGlobal>
#include <KStandardDirs>
#include <KWallet/Wallet>
#include <KDirWatch>
#include <QDBusConnection>
#include <QDBusAbstractAdaptor>

using namespace KWallet;
class testRemoveNetAttachJob : public QObject
{
    Q_OBJECT

    public:
        explicit testRemoveNetAttachJob(QObject* parent = 0);

    private Q_SLOTS:
        void testRemove();

    private:
        Wallet *m_wallet;
};


testRemoveNetAttachJob::testRemoveNetAttachJob(QObject* parent) : QObject(parent)
{
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");
    QString destPath = KGlobal::dirs()->saveLocation("remote_entries");
    destPath.append("username_host.com.desktop");

    QFile file(destPath);
    file.open(QIODevice::WriteOnly);
    file.write("FooBarFo");
    file.close();

    QMap<QString, QString> data;
    data["login"] = "username";
    data["password"] = "password";

    m_wallet = Wallet::openWallet(Wallet::NetworkWallet(), 0, Wallet::Synchronous);
    m_wallet->setFolder("Passwords");
    m_wallet->writeMap("webdav-username@host.com:-1", data);
}

void testRemoveNetAttachJob::testRemove()
{
    KGlobal::dirs()->addResourceType("remote_entries", "data", "remoteview");
    QString destPath = KGlobal::dirs()->saveLocation("remote_entries");
    destPath.append("username_host.com.desktop");

    RemoveNetAttachJob *job = new RemoveNetAttachJob(this);
    job->setHost("host.com");
    job->setUsername("username");
    job->exec();

    QVERIFY2(!QFile::exists(destPath), "Desktop file has not been deleted");
    QVERIFY2(!m_wallet->hasEntry("webdav-username@host.com:-1"), "Wallet still exists");
}

QTEST_KDEMAIN_CORE(testRemoveNetAttachJob)

#include "testremovenetattachjob.moc"
