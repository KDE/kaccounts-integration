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

#include "faccountwidget.h"

#include "google/pages/serviceoption.h"
#include "facebook/jobs/fcreatepim.h"
#include "facebook/jobs/fcreatechat.h"
#include "google/jobs/removechat.h"
#include "jobs/removeakonadiresource.h"

#include <QtCore/QDebug>

FAccountWidget::FAccountWidget(KConfigGroup group, QWidget* parent)
 : QWidget(parent)
 , m_config(group)
{
    setupUi(this);

    int status = 0;
    KConfigGroup services = m_config.group("services");
    QStringList keys = services.keyList();
    Q_FOREACH(const QString &key, keys) {
        status = services.readEntry(key, 0);
        m_serviceWidgets[key] = new ServiceOption(key, key, this);
        m_serviceWidgets[key]->setStatus(status);
        connect(m_serviceWidgets[key], SIGNAL(toggled(QString,bool)), this,
SLOT(serviceChanged(QString,bool)));
        d_layout->addWidget(m_serviceWidgets[key]);
    }
}

FAccountWidget::~FAccountWidget()
{

}

void FAccountWidget::serviceChanged(const QString& service, bool enabled)
{
    if (service == "PIM") {
        modifyPIM(enabled);
        return;
    }

    if (service == "Chat") {
        modifyChat(enabled);
        return;
    }
    qWarning() << "Not implemented service: " << service;
}

void FAccountWidget::updateService(const QString& name)
{
    int status = m_config.group("services").readEntry(name, -1);
    m_serviceWidgets[name]->setStatus(status);
}

void FAccountWidget::modifyPIM(bool enabled)
{
    if (!enabled) {
        RemoveAkonadiResource *removePIM = new RemoveAkonadiResource("facebookResource", "PIM",
m_config, this);
        connect(removePIM, SIGNAL(finished(KJob*)), this, SLOT(updatePIM()));
        removePIM->start();
        return;
    }

    FCreatePIM *createCalendar = new FCreatePIM(m_config, this);
    connect(createCalendar, SIGNAL(finished(KJob*)), this, SLOT(updatePIM()));
    createCalendar->start();
}

void FAccountWidget::modifyChat(bool enabled)
{
    if (!enabled) {
        RemoveChat *removeChat = new RemoveChat(m_config, this);
        connect(removeChat, SIGNAL(finished(KJob*)), this, SLOT(updateChat()));
        removeChat->start();
        return;
    }

    FCreateChat *createChat = new FCreateChat(m_config, this);
    connect(createChat, SIGNAL(finished(KJob*)), this, SLOT(updateChat()));
    createChat->start();
}

void FAccountWidget::updatePIM()
{
    updateService("PIM");
}

void FAccountWidget::updateChat()
{
    updateService("Chat");
}

void FAccountWidget::updateAll()
{
    KConfigGroup services = m_config.group("services");
    QStringList keys = services.keyList();
    Q_FOREACH(const QString &key, keys) {
        updateService(key);
    }
}