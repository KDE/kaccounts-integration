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

#ifndef CHANGE_SETTINGS_JOB_H
#define CHANGE_SETTINGS_JOB_H

#include <QDBusMessage>

#include <KJob>
#include <Akonadi/AgentInstance>
#include <Accounts/Account>

class QDBusPendingCallWatcher;
class ChangeSettingsJob : public KJob
{
    Q_OBJECT
public:
    explicit ChangeSettingsJob(QObject* parent = 0);
    virtual ~ChangeSettingsJob();

    virtual void start();

    void setAccountId(const Accounts::AccountId &accId);
    void setResourceId(const QString &resourceId);

    void setSetting(const QString &key, const QVariant &value);

private Q_SLOTS:
    void init();
    void dbusSettingsPath(KJob *job);
    void accountSet(QDBusPendingCallWatcher* watcher);
    void configWritten(QDBusPendingCallWatcher* watcher);

private:
    QDBusMessage createCall(const QString &method);
    void setConfiguration();
    void writeConfig();

    QString m_resourceId;
    QString m_key;
    QString m_interface;
    QVariant m_value;
    Accounts::AccountId m_accountId;
};

#endif //CHANGE_SETTINGS_JOB_H