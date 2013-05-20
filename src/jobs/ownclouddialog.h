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

#ifndef OWNCLOUD_DIALOG_H
#define OWNCLOUD_DIALOG_H

#include "ui_owncloudDialog.h"
#include <KDialog>

namespace KIO
{
    class Job;
};
class KJob;
class KPixmapSequenceOverlayPainter;
class OwncloudDialog : public KDialog, Ui::owncloudDialog
{
    Q_OBJECT

    public:
        explicit OwncloudDialog(QWidget* parent = 0, Qt::WindowFlags flags = 0);

    public Q_SLOTS:
        void checkAuth();
        void checkServer(const QString &host);
        void fileChecked(KJob* job);
        void dataReceived(KIO::Job *job, const QByteArray &data);
        void authChecked(KJob *job);
    private:
        void checkServer(const KUrl &url);
        void figureOutServer(const QString& urlStr);
        void setWorking(bool start);
        void setResult(bool result);

        bool m_validHost;
        KUrl m_server;
        QByteArray m_json;
        KPixmapSequenceOverlayPainter *m_painter;
};

#endif //OWNCLOUD_DIALOG_H