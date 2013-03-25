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

#include <QtCore/QCoreApplication>

FakeResource* FakeResource::s_instance = 0;

FakeResource* FakeResource::self()
{
    if (!s_instance) {
        s_instance = new FakeResource(qApp);
    }
    return s_instance;
}

int FakeResource::accountId()
{
    return m_accountId;
}

void FakeResource::setAccountId(int accountId)
{
    m_accountId = accountId;
}

void FakeResource::writeConfig()
{
    Q_EMIT configWritten();
}

FakeResource::FakeResource(QObject* parent)
: QDBusAbstractAdaptor(parent)
, m_accountId(42)
{
    QString path = "/Settings";

    QDBusConnection conn = QDBusConnection::sessionBus();
    conn.registerService("org.freedesktop.Akonadi.Agent.akonadi_fake_resource_116");
    conn.registerService("org.freedesktop.Akonadi.Resource.akonadi_fake_resource_116");
    conn.registerObject(path, this, QDBusConnection::ExportAllContents);
}

#include "fakeresource.moc"