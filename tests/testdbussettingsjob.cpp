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

#include <QtTest/QtTest>

#include "../src/daemon/dbussettingspathjob.h"

#include <QDBusConnection>
#include <QDBusAbstractAdaptor>
#include <unistd.h>

class FakeAkonadiResource : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Akonadi.fakeResource.Settings")

    public:
        explicit FakeAkonadiResource(QObject* parent = 0);

    public Q_SLOTS:
        void setAccountId(int accountId);
};

FakeAkonadiResource::FakeAkonadiResource(QObject* parent) : QDBusAbstractAdaptor(parent)
{

}

void FakeAkonadiResource::setAccountId(int accountId)
{
    qDebug() << "AccId: " << accountId;
}

class testDBusSettingsJob : public QObject
{
    Q_OBJECT

    public:
        explicit testDBusSettingsJob(QObject* parent = 0);

    private Q_SLOTS:
        void testFindPath();
};

testDBusSettingsJob::testDBusSettingsJob(QObject* parent): QObject(parent)
{
    QString path = "/Settings";

    QDBusConnection conn = QDBusConnection::sessionBus();
    conn.registerService("org.freedesktop.Akonadi.Agent.akonadi_fake_resource_116");
    conn.registerService("org.freedesktop.Akonadi.Resource.akonadi_fake_resource_116");
    conn.registerObject(path, new FakeAkonadiResource(qApp), QDBusConnection::ExportAllContents);
}

void testDBusSettingsJob::testFindPath()
{
    DBusSettingsPathJob *job = new DBusSettingsPathJob(this);

    job->setResourceId("akonadi_fake_resource_116");
    job->exec();

    QVERIFY2(!job->error(), "Job is set as finished with error");
    QVERIFY2(!job->path().isEmpty(), "The path is empty");
    QCOMPARE(job->path(), QLatin1String("fakeResource"));

    job = new DBusSettingsPathJob(this);

    job->setResourceId("akonadi_fake_resource_notfound");
    job->exec();

    QVERIFY2(job->error(), "There should be an error since the resouce does not exists");
    QVERIFY2(job->path().isEmpty(), "The path should be empty");
}

QTEST_MAIN(testDBusSettingsJob)

#include "testdbussettingsjob.moc"
