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
#include "oaccountwidget.h"

#include "google/pages/serviceoption.h"
#include "owncloud/jobs/ocreatecontact.h"
#include "owncloud/jobs/ocreatecalendar.h"
#include "owncloud/jobs/ocreatefile.h"
#include "owncloud/jobs/oremovefile.h"
#include "owncloud/jobs/oremovecalendar.h"
#include "owncloud/jobs/oremovecontact.h"

#include <QDebug>

OAccountWidget::OAccountWidget(KConfigGroup group, QWidget *parent)
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
        connect(m_serviceWidgets[key], SIGNAL(toggled(QString,bool)),
                this, SLOT(serviceChanged(QString,bool)));
        d_layout->addWidget(m_serviceWidgets[key]);
    }
}

OAccountWidget::~OAccountWidget()
{

}

void OAccountWidget::serviceChanged(const QString &service, bool enabled)
{
    if (service == "Calendar") {
        modifyCalendar(enabled);
        return;
    }

    if (service == "Contact") {
        modifyContact(enabled);
        return;
    }

    if (service == "File") {
        modifyFile(enabled);
        return;
    }

    qWarning() << "Not implemented service: " << service;
}

void OAccountWidget::updateService(const QString &name)
{
    int status = m_config.group("services").readEntry(name, -1);
    m_serviceWidgets[name]->setStatus(status);
}

void OAccountWidget::modifyCalendar(bool enabled)
{
    if (!enabled) {
        ORemoveCalendar *removeCalendar = new ORemoveCalendar(m_config, this);
        connect(removeCalendar, SIGNAL(finished(KJob*)), this, SLOT(updateCalendar()));
        removeCalendar->start();
        return;
    }

    OCreateCalendar *createCalendar = new OCreateCalendar(m_config, this);
    connect(createCalendar, SIGNAL(finished(KJob*)), this, SLOT(updateCalendar()));
    createCalendar->start();
}

void OAccountWidget::modifyFile(bool enabled)
{
    if (!enabled) {
        ORemoveFile *removeFile = new ORemoveFile(m_config, this);
        connect(removeFile, SIGNAL(finished(KJob*)), this, SLOT(updateFile()));
        removeFile->start();
        return;
    }

    OCreateFile *createFile = new OCreateFile(m_config, this);
    connect(createFile, SIGNAL(finished(KJob*)), this, SLOT(updateFile()));
    createFile->start();
}

void OAccountWidget::modifyContact(bool enabled)
{
    if (!enabled) {
        ORemoveContact *removeContact = new ORemoveContact(m_config, this);
        connect(removeContact, SIGNAL(finished(KJob*)), this, SLOT(updateContact()));
        removeContact->start();
        return;
    }

    OCreateContact *createContact = new OCreateContact(m_config, this);
    connect(createContact, SIGNAL(finished(KJob*)), this, SLOT(updateContact()));
    createContact->start();
}

void OAccountWidget::updateCalendar()
{
    updateService("Calendar");
}

void OAccountWidget::updateFile()
{
    updateService("File");
}

void OAccountWidget::updateContact()
{
    updateService("Contact");
}

void OAccountWidget::updateAll()
{
    KConfigGroup services = m_config.group("services");
    QStringList keys = services.keyList();

    Q_FOREACH(const QString &key, keys) {
        updateService(key);
    }
}
