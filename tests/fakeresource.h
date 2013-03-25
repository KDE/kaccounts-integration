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

#ifndef FAKE_RESOURCE_H
#define FAKE_RESOURCE_H

#include <QtCore/QStringList>

#include <QDBusAbstractAdaptor>
#include <QDBusConnection>

class FakeResource : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Akonadi.fakeResource.Settings")

    public:
        static FakeResource* self();

    public Q_SLOTS:
        int accountId();
        void setAccountId(int accountId);
        QStringList accountServices();
        void setAccountServices(const QStringList &services);
        void writeConfig();

    Q_SIGNALS:
        void configWritten();

    private:
        FakeResource(QObject* parent = 0);

        int m_accountId;
        QStringList m_accountServices;
        static FakeResource *s_instance;
};

#endif //FAKE_RESOURCE_H