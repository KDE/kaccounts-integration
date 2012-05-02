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

#include "connecting.h"
#include "owncloud.h"

#include <KDebug>

#include <KPixmapSequenceOverlayPainter>
#include <KIO/Job>

Connecting::Connecting(OwnCloudWizard* parent)
 : QWizardPage(parent)
 , m_wizard(parent)
{
    setupUi(this);
    KPixmapSequenceOverlayPainter *painter = new KPixmapSequenceOverlayPainter(this);
    painter->setWidget(working);
    painter->start();
}

Connecting::~Connecting()
{

}

void Connecting::initializePage()
{
    QList <QWizard::WizardButton> list;
    list << QWizard::Stretch;
    list << QWizard::BackButton;
    list << QWizard::NextButton;
    list << QWizard::CancelButton;
    m_wizard->setButtonLayout(list);

    server->setText(m_wizard->server().host());

    QMetaObject::invokeMethod(this, "checkAuth", Qt::QueuedConnection);
}

void Connecting::checkAuth()
{
    KUrl url(m_wizard->server());

    url.setUser(m_wizard->username());
    url.setPass(m_wizard->password());

    url.addPath("apps/calendar/caldav.php/");
    kDebug() << "FinalUrL: " << url;
    KIO::TransferJob *job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(httpResult(KJob*)));

    job->setUiDelegate(0);
}

void Connecting::httpResult(KJob* job)
{
    if (job->error()) {
        kDebug() << job->errorString();
        kDebug() << job->errorText();
    }

    KIO::TransferJob *kJob = qobject_cast<KIO::TransferJob*>(job);
    if (kJob->isErrorPage()) {
        error->setText(i18n("User or password are incorrect"));
        return;
    }

    m_wizard->next();
}