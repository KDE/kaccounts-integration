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

#include "create.h"
#include "google/google.h"
#include "facebook/facebook.h"
#include "owncloud/owncloud.h"

#include "ui_types.h"

#include <QtCore/QDebug>

#include <QtGui/QWidget>

#include <libkgoogle/auth.h>
#include <libkgoogle/services/tasks.h>
#include <libkgoogle/services/contacts.h>
#include <libkgoogle/services/calendar.h>

using namespace KGoogle;

Create::Create(QWidget* parent)
: QObject(parent)
, m_form(0)
{
    m_parent = parent;
}

Create::~Create()
{

}

QWidget* Create::widget()
{
    if (!m_form) {
        m_form = new Ui::createForm();
    }

    QWidget *widget = new QWidget(m_parent);
    m_form->setupUi(widget);

    QMetaObject::invokeMethod(this, "stablishConnections", Qt::QueuedConnection);

    return widget;
}

void Create::stablishConnections()
{
    connect(m_form->googleBtn, SIGNAL(clicked(bool)), this, SLOT(startGoogle()));
    connect(m_form->facebookBtn, SIGNAL(clicked(bool)), this, SLOT(startFacebook()));
    connect(m_form->owncloudBtn, SIGNAL(clicked(bool)), this, SLOT(startOwncloud()));
}

void Create::startGoogle()
{
    GoogleWizard *wizard = new GoogleWizard(m_parent);
    connect(wizard, SIGNAL(newAccount(QString,QString)), this, SIGNAL(newAccount(QString,QString)));
    wizard->setModal(true);
    wizard->show();
}

void Create::startFacebook()
{
    FacebookWizard *wizard = new FacebookWizard(m_parent);
    connect(wizard, SIGNAL(newAccount(QString,QString)), this, SIGNAL(newAccount(QString,QString)));
    wizard->setModal(true);
    wizard->show();
}

void Create::startLive()
{
    qWarning("Live not implemented yet");
}

void Create::startOwncloud()
{
    OwnCloudWizard *wizard = new OwnCloudWizard(m_parent);
    connect(wizard, SIGNAL(newAccount(QString,QString)), this, SIGNAL(newAccount(QString,QString)));
    wizard->setModal(true);
    wizard->show();
}
