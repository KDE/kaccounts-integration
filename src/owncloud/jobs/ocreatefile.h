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

#ifndef OCREATEFILE_H
#define OCREATEFILE_H

#include <KJob>
#include <KConfigGroup>

class OCreateFile : public KJob
{
    Q_OBJECT
    public:
        explicit OCreateFile(KConfigGroup group, QObject* parent = 0);
        virtual ~OCreateFile();

        virtual void start();

    private Q_SLOTS:
        void getRealm();
        void gotRealm(KJob* job);
        void createNetAttach();

    private:
        QString m_realm;
        KConfigGroup m_config;
};

#endif //OCREATEFILE_H