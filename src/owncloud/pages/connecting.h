/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
 *  Copyright (C) 2015 by Martin Klapetek <mklapetek@kde.org>                        *
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

#ifndef CONNECTING_H
#define CONNECTING_H

#include "ui_connecting.h"

#include <QWizardPage>
#include <KJob>

class KJob;

class Connecting : public QWizardPage, Ui::Connecting
{
    Q_OBJECT

public:
    explicit Connecting(QWizard *parent);
    virtual ~Connecting();

    virtual void initializePage();

private Q_SLOTS:
    void checkAuth();
    void httpResult(KJob *job);

private:
    QWizard *m_wizard;
};

#endif //CONNECTING_H
