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

#include "google/pages/serviceoption.h"
#include "google/jobs/createcontact.h"
#include "google/jobs/createcalendar.h"
#include "google/jobs/createtask.h"
#include "google/jobs/createmail.h"
#include "google/jobs/createchat.h"
#include "google/jobs/removechat.h"
#include "google/jobs/removeemail.h"
#include "google/jobs/removecalendar.h"
#include "google/jobs/removetask.h"
#include "jobs/removeakonadiresource.h"

#include <accountwidget.h>

#include <QtCore/QDebug>

AccountWidget::AccountWidget(KConfigGroup group, QWidget* parent)
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
        connect(m_serviceWidgets[key], SIGNAL(toggled(QString,bool)), this, SLOT(serviceChanged(QString,bool)));
        d_layout->addWidget(m_serviceWidgets[key]);
    }
}

AccountWidget::~AccountWidget()
{

}

void AccountWidget::serviceChanged(const QString& service, bool enabled)
{
    if (service == "Calendar") {
        modifyCalendar(enabled);
        return;
    }

    if (service == "Contact") {
        modifyContact(enabled);
        return;
    }

    if (service == "EMail") {
        modifyEMail(enabled);
        return;
    }

    if (service == "Chat") {
        modifyChat(enabled);
        return;
    }

    if (service == "Tasks") {
        modifyTasks(enabled);
        return;
    }

    qWarning() << "Not implemented service: " << service;
}

void AccountWidget::updateService(const QString& name)
{
    int status = m_config.group("services").readEntry(name, -1);
    m_serviceWidgets[name]->setStatus(status);
}

void AccountWidget::modifyCalendar(bool enabled)
{
    if (!enabled) {
        RemoveCalendar *removeCalendar = new RemoveCalendar(m_config, this);
        connect(removeCalendar, SIGNAL(finished(KJob*)), this, SLOT(updateCalendar()));
        removeCalendar->start();
        return;
    }

    CreateCalendar *createCalendar = new CreateCalendar(m_config, this);
    connect(createCalendar, SIGNAL(finished(KJob*)), this, SLOT(updateCalendar()));
    createCalendar->start();
}

void AccountWidget::modifyChat(bool enabled)
{
    if (!enabled) {
        RemoveChat *removeChat = new RemoveChat(m_config, this);
        connect(removeChat, SIGNAL(finished(KJob*)), this, SLOT(updateChat()));
        removeChat->start();
        return;
    }

    CreateChat *createChat = new CreateChat(m_config, this);
    connect(createChat, SIGNAL(finished(KJob*)), this, SLOT(updateChat()));
    createChat->start();
}

void AccountWidget::modifyContact(bool enabled)
{
    if (!enabled) {
        RemoveAkonadiResource *removeContact = new RemoveAkonadiResource("contactResource", "Contact", m_config, this);
        connect(removeContact, SIGNAL(finished(KJob*)), this, SLOT(updateContact()));
        removeContact->start();
        return;
    }

    CreateContact *createContact = new CreateContact(m_config, this);
    connect(createContact, SIGNAL(finished(KJob*)), this, SLOT(updateContact()));
    createContact->start();
}

void AccountWidget::modifyEMail(bool enabled)
{
    if (!enabled) {
        RemoveEmail *removeEMail = new RemoveEmail(m_config, this);
        connect(removeEMail, SIGNAL(finished(KJob*)), this, SLOT(updateMail()));
        removeEMail->start();
        return;
    }

    CreateMail *createEMail = new CreateMail(m_config, this);
    connect(createEMail, SIGNAL(finished(KJob*)), this, SLOT(updateMail()));
    createEMail->start();
}

void AccountWidget::modifyTasks(bool enabled)
{
    if (!enabled) {
        RemoveTask *removeTask = new RemoveTask(m_config, this);
        connect(removeTask, SIGNAL(finished(KJob*)), this, SLOT(updateTask()));
        removeTask->start();
        return;
    }

    CreateTask *createTasks = new CreateTask(m_config, this);
    connect(createTasks, SIGNAL(finished(KJob*)), this, SLOT(updateTask()));
    createTasks->start();
}

void AccountWidget::updateCalendar()
{
    updateService("Calendar");
}

void AccountWidget::updateChat()
{
    updateService("Chat");
}

void AccountWidget::updateContact()
{
    updateService("Contact");
}

void AccountWidget::updateMail()
{
    updateService("EMail");
}

void AccountWidget::updateTask()
{
    updateService("Tasks");
}

void AccountWidget::updateAll()
{
    KConfigGroup services = m_config.group("services");
    QStringList keys = services.keyList();
    Q_FOREACH(const QString &key, keys) {
        updateService(key);
    }
}