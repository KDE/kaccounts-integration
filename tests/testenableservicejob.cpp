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

#include "fakeresource.h"
#include "../src/daemon/akonadi/jobs/enableservicejob.h"

#include <QtTest/QtTest>

#include <QDBusConnection>
#include <QDBusAbstractAdaptor>

#include <Accounts/Provider>
#include <Accounts/Manager>

class testEnableServiceJob : public QObject
{
    Q_OBJECT
    public:
        explicit testEnableServiceJob(QObject* parent = 0);

    private Q_SLOTS:
        void initTestCase();
        void testEnableServiceFromEmpty();
        void testAlreadyEnabledService();
        void testEnableWhenNotEmpty();
        void testDisableWhenNoEmpty();
        void testDisableLast();

    private:
        Accounts::Service m_service;
        Accounts::Service m_service2;
};

void testEnableServiceJob::initTestCase()
{
    setenv ("AG_SERVICES", TEST_DATA, false);
    setenv ("AG_SERVICE_TYPES", TEST_DATA, false);
    setenv ("AG_PROVIDERS", TEST_DATA, false);
    Accounts::Manager manager;
    m_service = manager.service("MyService");
    m_service2 = manager.service("MyService2");
}

testEnableServiceJob::testEnableServiceJob(QObject* parent): QObject(parent)
{
    FakeResource::self();
}

void testEnableServiceJob::testEnableServiceFromEmpty()
{
    EnableServiceJob *job = new EnableServiceJob(this);
    job->setResourceId("akonadi_fake_resource_116");
    job->setInterface("org.kde.Akonadi.fakeResource.Settings");
    job->addService(m_service, EnableServiceJob::Enable);
    job->exec();

    QStringList services = FakeResource::self()->accountServices();
    QCOMPARE(services.count(), 1);
    QCOMPARE(services.first(), QLatin1String("MyService"));
}

void testEnableServiceJob::testAlreadyEnabledService()
{
    EnableServiceJob *job = new EnableServiceJob(this);
    job->setResourceId("akonadi_fake_resource_116");
    job->setInterface("org.kde.Akonadi.fakeResource.Settings");
    job->addService(m_service, EnableServiceJob::Enable);
    job->exec();

    QStringList services = FakeResource::self()->accountServices();
    QCOMPARE(services.count(), 1);
    QCOMPARE(services.first(), QLatin1String("MyService"));
}

void testEnableServiceJob::testEnableWhenNotEmpty()
{
    EnableServiceJob *job = new EnableServiceJob(this);
    job->setResourceId("akonadi_fake_resource_116");
    job->setInterface("org.kde.Akonadi.fakeResource.Settings");
    job->addService(m_service2, EnableServiceJob::Enable);
    job->exec();

    QStringList services = FakeResource::self()->accountServices();
    QCOMPARE(services.count(), 2);
    QVERIFY2(services.contains("MyService2"), "MyService2 has been enabled but it is not in the resource");
}

void testEnableServiceJob::testDisableWhenNoEmpty()
{
    EnableServiceJob *job = new EnableServiceJob(this);
    job->setResourceId("akonadi_fake_resource_116");
    job->setInterface("org.kde.Akonadi.fakeResource.Settings");
    job->addService(m_service, EnableServiceJob::Disable);
    job->exec();

    QStringList services = FakeResource::self()->accountServices();
    QCOMPARE(services.count(), 1);
    QVERIFY2(!services.contains("MyService"), "MyService has been disabled but it is still in the resource");
}

void testEnableServiceJob::testDisableLast()
{
    EnableServiceJob *job = new EnableServiceJob(this);
    job->setResourceId("akonadi_fake_resource_116");
    job->setInterface("org.kde.Akonadi.fakeResource.Settings");
    job->addService(m_service2, EnableServiceJob::Disable);
    job->exec();

    QStringList services = FakeResource::self()->accountServices();

    QString error("Job is set as finished with error: ");
    error.append(job->errorText());


    QVERIFY2(!job->error(), error.toUtf8().data());
    QVERIFY2(services.isEmpty(), "Resource has services though all of them have been disabled");
}

QTEST_MAIN(testEnableServiceJob)

#include "testenableservicejob.moc"
