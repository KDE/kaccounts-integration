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
#include "runnerid/runnerid.h"

#include "ui_types.h"

#include <QtCore/QDebug>

#include <QtGui/QWidget>
#include <QtGui/QCommandLinkButton>

#include <Accounts/Manager>
#include <Accounts/Provider>

#include <libkgapi/auth.h>
#include <libkgapi/services/tasks.h>
#include <libkgapi/services/contacts.h>
#include <libkgapi/services/calendar.h>

using namespace KGAPI;

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
        m_form = new Ui_createForm();
    }

    QWidget *widget = new QWidget(m_parent);
    m_form->setupUi(widget);

    QMetaObject::invokeMethod(this, "fillInterface", Qt::QueuedConnection);

    return widget;
}

void Create::fillInterface()
{
    Accounts::Manager *manager = new Accounts::Manager(this);
    Accounts::ProviderList providerList = manager->providerList();

    QCommandLinkButton *button;
    Q_FOREACH(const Accounts::Provider& provider, providerList) {
        button = new QCommandLinkButton(provider.displayName());
        button->setIcon(KIcon(provider.iconName()));

        m_form->verticalLayout->addWidget(button);
    }
}
