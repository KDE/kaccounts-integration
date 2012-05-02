/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#ifndef BASIC_INFO_H
#define BASIC_INFO_H

#include "ui_basicinfo.h"

#include <QWizardPage>
#include <KDE/KIO/AccessManager>

namespace KIO
{
    class Job;
};
class KJob;
class OwnCloudWizard;
class KPixmapSequenceOverlayPainter;
class BasicInfo : public QWizardPage, Ui_Owncloud
{
    Q_OBJECT
    public:
        BasicInfo(OwnCloudWizard *parent);
        virtual ~BasicInfo();

        virtual void initializePage();
        virtual bool validatePage();
        virtual bool isComplete() const;

    private Q_SLOTS:
        void checkServer();
        void fileChecked(KJob* job);
        void dataReceived(KIO::Job *job, const QByteArray &data);

    private:
        bool validData() const;
        void checkServer(const QString &path);
        void checkServer(const KUrl &url);
        void figureOutServer(const QString& urlStr);

        void setWorking(bool start);
        void setResult(bool result);

    private:
        KUrl m_server;
        bool m_validServer;
        QByteArray m_json;
        KPixmapSequenceOverlayPainter *m_painter;
        OwnCloudWizard *m_wizard;
};

#endif //BASIC_INFO_H