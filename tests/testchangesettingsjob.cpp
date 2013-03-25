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

#include "../src/daemon/jobs/changesettingsjob.h"

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
    int accountId();
    void setAccountId(int accountId);
    void writeConfig();

Q_SIGNALS:
    void configWritten();

private:
    int m_accountId;
};

FakeAkonadiResource::FakeAkonadiResource(QObject* parent) : QDBusAbstractAdaptor(parent)
{
    m_accountId = 42;
}

int FakeAkonadiResource::accountId()
{
    return m_accountId;
}

void FakeAkonadiResource::setAccountId(int accountId)
{
    m_accountId = accountId;
}

void FakeAkonadiResource::writeConfig()
{
    Q_EMIT configWritten();
}

class testChangeSettingsJob : public QObject
{
    Q_OBJECT

public:
    explicit testChangeSettingsJob(QObject* parent = 0);

private Q_SLOTS:
    void testChangeSettings();

private:
    FakeAkonadiResource* m_fakeResource;
};

testChangeSettingsJob::testChangeSettingsJob(QObject* parent): QObject(parent)
{
    QString path = "/Settings";

    m_fakeResource = new FakeAkonadiResource(this);
    QDBusConnection conn = QDBusConnection::sessionBus();
    conn.registerService("org.freedesktop.Akonadi.Agent.akonadi_fake_resource_116");
    conn.registerService("org.freedesktop.Akonadi.Resource.akonadi_fake_resource_116");
    conn.registerObject(path, m_fakeResource, QDBusConnection::ExportAllContents);
}

void testChangeSettingsJob::testChangeSettings()
{
    ChangeSettingsJob *job = new ChangeSettingsJob(this);
    QSignalSpy signalSpy(m_fakeResource, SIGNAL(configWritten()));

    job->setResourceId("akonadi_fake_resource_116");
    job->setSetting("setAccountId", 23);
    job->exec();

    QVERIFY2(!job->error(), "Job is set as finished with error");
    QCOMPARE(m_fakeResource->accountId(), 23);
    QCOMPARE(signalSpy.count(), 1);
}

QTEST_MAIN(testChangeSettingsJob)

#include "testchangesettingsjob.moc"
