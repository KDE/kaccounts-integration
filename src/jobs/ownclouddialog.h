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

class KJob;
class QTimer;
class KPixmapSequenceOverlayPainter;
class OwncloudDialog : public KDialog, Ui::owncloudDialog
{
    Q_OBJECT

    public:
        explicit OwncloudDialog(QWidget* parent = 0, Qt::WindowFlags flags = 0);

    public Q_SLOTS:
        void checkAuth();
        void checkServer();
        void hostChanged();
        void authChanged();
        void authChecked(KJob* job);
        void hostChecked(KJob* job);

    private:
        enum Type {
            Host,
            Auth
        };
        void setWorking(bool start, Type type);
        void setResult(bool result, Type type);

        QTimer *m_timerHost;
        QTimer *m_timerAuth;
        QString m_url;
        KPixmapSequenceOverlayPainter *m_painter;
};

#endif //OWNCLOUD_DIALOG_H