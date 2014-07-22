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

#ifndef CHECK_OWNCLOUD_HOST_JOB_H
#define CHECK_OWNCLOUD_HOST_JOB_H

#include <KJob>

namespace KIO
{
    class Job;
};

class QUrl;
class CheckOwncloudHostJob : public KJob
{
    Q_OBJECT
    public:
        explicit CheckOwncloudHostJob(QObject* parent = 0);
        virtual ~CheckOwncloudHostJob();

        virtual void start();

        QString url() const;
        void setUrl(const QString &url);

    private Q_SLOTS:
        void requestStatus();
        void dataReceived(KIO::Job* job, const QByteArray& data);
        void fileDownloaded(KJob *job);

    private:
        void figureOutServer(const QUrl &url);

        QString m_url;
        QByteArray m_json;
};

#endif //CHECK_OWNCLOUD_HOST_JOB_H