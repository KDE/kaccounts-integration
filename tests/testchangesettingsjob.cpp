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

#include <QtTest/QtTest>

#include "../src/daemon/jobs/changesettingsjob.h"

#include <QDBusConnection>
#include <QDBusAbstractAdaptor>
#include <unistd.h>

class testChangeSettingsJob : public QObject
{
    Q_OBJECT

public:
    explicit testChangeSettingsJob(QObject* parent = 0);

private Q_SLOTS:
    void testChangeSettings();
};

testChangeSettingsJob::testChangeSettingsJob(QObject* parent): QObject(parent)
{
}

void testChangeSettingsJob::testChangeSettings()
{
    ChangeSettingsJob *job = new ChangeSettingsJob(this);
    QSignalSpy signalSpy(FakeResource::self(), SIGNAL(configWritten()));

    job->setResourceId("akonadi_fake_resource_116");
    job->setSetting("setAccountId", 23);
    job->exec();

    QVERIFY2(!job->error(), "Job is set as finished with error");
    QCOMPARE(FakeResource::self()->accountId(), 23);
    QCOMPARE(signalSpy.count(), 1);
}

QTEST_MAIN(testChangeSettingsJob)

#include "testchangesettingsjob.moc"
