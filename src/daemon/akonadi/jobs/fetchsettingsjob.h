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

#ifndef FETCH_SETTINGS_JOB_H
#define FETCH_SETTINGS_JOB_H

#include "abstractakonadijob.h"

#include <QtCore/QVariant>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>
#include <QDebug>

class FetchSettingsJob : public AbstractAkonadiJob
{
    Q_OBJECT

public:
    explicit FetchSettingsJob(QObject *parent = 0);
    virtual ~FetchSettingsJob();

    virtual void start();

    template< typename T>
    T value() const
    {
        QDBusPendingReply<T> reply = *m_watcher;
        if (reply.isError()) {
            qDebug() << reply.error().message();
            return T();
        }

        return reply.value();
    }

    QString key() const;
    void setKey(const QString &key);

private Q_SLOTS:
    void init();
    void dbusSettingsPathDone(KJob *job);
    void fetchDone(QDBusPendingCallWatcher*);

private:
    void fetchSettings();

    QString m_key;
    QDBusPendingCallWatcher *m_watcher;
};

#endif //FETCH_SETTINGS_JOB_H
