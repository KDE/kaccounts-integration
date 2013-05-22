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

#include "../src/daemon/akonadi/akonadiaccounts.h"

#include <QtTest/QtTest>

#include <KGlobal>
#include <KComponentData>
#include <KStandardDirs>
#include <KConfigGroup>

class testAkonadiAccounts : public QObject
{
    Q_OBJECT
    public:
        explicit testAkonadiAccounts(QObject* parent = 0);
        virtual ~testAkonadiAccounts();

    private Q_SLOTS:
        void testResourceWhenEmpty();
        void testAddResource();
        void testResources();
        void testHasServices();
        void testResourceFromType();
        void testResource();
        void testRemoveResource();

    private:
        QString m_path;
        QString m_resourceId;
        KSharedConfig::Ptr m_config;
        AkonadiAccounts *m_accounts;
};

testAkonadiAccounts::testAkonadiAccounts(QObject* parent)
 : QObject(parent)
 , m_resourceId("akonadi_fake_resource_116")
{
    QString config("akonadiaccountstest");
    m_path = KGlobal::mainComponent().dirs()->saveLocation("config", QString(), false) + config;
    QFile::remove(m_path);

    m_config = KSharedConfig::openConfig(config);
    m_accounts = new AkonadiAccounts(config);
}

testAkonadiAccounts::~testAkonadiAccounts()
{
    QFile::remove(m_path);
}

void testAkonadiAccounts::testResourceWhenEmpty()
{
    QCOMPARE(m_accounts->resourceFromType(10, "akonadi_fake"), QString());
}

void testAkonadiAccounts::testAddResource()
{
    m_accounts->addService(10, "test-contacts", m_resourceId);
    QVERIFY2(m_config->hasGroup("Account_10"), "Group has not been created");

    KConfigGroup group = m_config->group("Account_10");
    QVERIFY2(!group.keyList().isEmpty(), "There are no keys in account group");
    QVERIFY2(group.hasKey("test-contacts"), "No key for test-contacts");
    QCOMPARE(group.readEntry("test-contacts", QString()), m_resourceId);

    m_accounts->addService(10, "test-contacts", m_resourceId);
    QVERIFY2(m_config->hasGroup("Account_10"), "Group does not exists");

    group = m_config->group("Account_10");
    QVERIFY2(!group.keyList().isEmpty(), "There are no keys in account group");
    QCOMPARE(group.keyList().count(), 1);
    QVERIFY2(group.hasKey("test-contacts"), "No key for akonadi_fake_resource_116");
    QCOMPARE(group.readEntry("test-contacts", QString()), m_resourceId);

    m_accounts->addService(10, "test-calendar", m_resourceId);
    group = m_config->group("Account_10");
    QVERIFY2(!group.keyList().isEmpty(), "There are no keys in account group");
    QCOMPARE(group.keyList().count(), 2);
    QVERIFY2(group.hasKey("test-calendar"), "No key for test-calendar");
    QCOMPARE(group.readEntry("test-calendar", QString()), m_resourceId);

    m_accounts->addService(11, "test-calendar", "akonadi_fake_resource_117");
    QVERIFY2(m_config->hasGroup("Account_11"), "Group Account_11 has not been created");

    group = m_config->group("Account_11");
    QVERIFY2(!group.keyList().isEmpty(), "There are no keys in account group");
    QCOMPARE(group.keyList().count(), 1);
    QVERIFY2(group.hasKey("test-calendar"), "No key for test-calendar");
    QCOMPARE(group.readEntry("test-calendar", QString()), QLatin1String("akonadi_fake_resource_117"));
}

void testAkonadiAccounts::testResources()
{
    QStringList resources = m_accounts->resources(10);
    QCOMPARE(resources.count(), 1);
}

void testAkonadiAccounts::testHasServices()
{
    QVERIFY(m_accounts->hasServices(10));
    QVERIFY(m_accounts->hasService(10, QLatin1String("test-calendar")));
    QVERIFY(!m_accounts->hasService(10, QLatin1String("notfound")));
    QVERIFY(!m_accounts->services(10).isEmpty());
    QCOMPARE(m_accounts->services(10).count(), 2);
}

void testAkonadiAccounts::testResourceFromType()
{
    QCOMPARE(m_accounts->resourceFromType(10, "akonadi_fake"), QLatin1String("akonadi_fake_resource_116"));
}

void testAkonadiAccounts::testResource()
{
    QCOMPARE(m_accounts->resource(10, "test-contacts"), QLatin1String("akonadi_fake_resource_116"));
    QCOMPARE(m_accounts->resource(10, "test-calendar"), QLatin1String("akonadi_fake_resource_116"));
    QCOMPARE(m_accounts->resource(11, "test-calendar"), QLatin1String("akonadi_fake_resource_117"));
}

void testAkonadiAccounts::testRemoveResource()
{
    m_accounts->removeService(10, "test-contacts");
    KConfigGroup group = m_config->group("Account_10");
    QVERIFY2(!group.keyList().isEmpty(), "There are no keys in account group");
    QVERIFY2(!group.hasKey("test-contacts"), "No key for test-contacts");
    QCOMPARE(group.keyList().count(), 1);
    QVERIFY2(group.hasKey("test-calendar"), "No key for test-calendar");
    QCOMPARE(group.readEntry("test-calendar", QString()), m_resourceId);

    m_accounts->removeService(10, "test-calendar");
    QVERIFY2(!group.hasGroup("Account_10"), "The account group has not been removed");
}

QTEST_MAIN(testAkonadiAccounts)

#include "testakonadiaccounts.moc"
