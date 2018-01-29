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
#include "jobs/createaccount.h"
#include <core.h>

#include "ui_types.h"

#include <QDebug>

#include <QWidget>
#include <QCommandLinkButton>
#include <QMessageBox>

#include <Accounts/Manager>
#include <Accounts/Provider>

#include <KLocalizedString>
#include <KMessageWidget>

Create::Create(QWidget *parent)
    : QObject(parent)
    , m_form(0)
    , m_manager(KAccounts::accountsManager())
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
    m_form->progressBar->hide();

    QMetaObject::invokeMethod(this, "fillInterface", Qt::QueuedConnection);

    return widget;
}

void Create::fillInterface()
{
    Accounts::ProviderList providerList = m_manager->providerList();
    const Accounts::ServiceList serviceList = m_manager->serviceList();

    if (providerList.isEmpty()) {
        auto noProvidersMessageWidget = new KMessageWidget(
                    xi18n("No account providers found.<nl/>"
                          "Some providers can be found in packages <application>kaccounts-providers</application>"
                          " and <application>ktp-accounts-kcm</application>."));
        noProvidersMessageWidget->setMessageType(KMessageWidget::MessageType::Warning);
        m_form->verticalLayout_2->insertWidget(0, noProvidersMessageWidget);
        return;
    }

    QSet<QString> wantedProviders;
    QSet<QString> locatedProviders;

    // sort accounts alphabetically
    std::sort(providerList.begin(), providerList.end(), [](const Accounts::Provider &a, const Accounts::Provider &b) {
        return QString::localeAwareCompare(b.displayName(), a.displayName()) < 0;
    });

    for(const Accounts::Provider &provider : providerList) {
        if (provider.name() == "ktp-generic") {
            locatedProviders.insert(provider.name());
            continue;
        }

        // Check if there are any services for the given provider
        // If not, just skip it and don't allow creating such account
        bool shouldSkip = true;
        for(const Accounts::Service &service : qAsConst(serviceList)) {
            qDebug() << "Checking service " << service.name() << " for provider " << provider.name();
            wantedProviders.insert(service.provider());
            if (service.provider() == provider.name()) {
                qDebug() << "Found a service, not skipping:" << service.name();
                locatedProviders.insert(provider.name());
                shouldSkip = false;
                break;
            }
        }

        if (shouldSkip) {
            continue;
        }

        createProviderButton(provider);
    }

    const auto missingProviders = wantedProviders.subtract(locatedProviders);
    if (!missingProviders.isEmpty()) {
        const auto missingProvidersString = missingProviders.toList().join(" ");
        const QString infoMessage = i18n("Following missing providers are required by installed services: %1", missingProvidersString);

        auto missingProvidersMessageWidget = new KMessageWidget(infoMessage);
        missingProvidersMessageWidget->setMessageType(KMessageWidget::MessageType::Warning);
        m_form->verticalLayout_2->insertWidget(0, missingProvidersMessageWidget);
        return;
    }
}

void Create::createProviderButton(const Accounts::Provider &provider)
{
    QCommandLinkButton *button = new QCommandLinkButton(i18nd(provider.trCatalog().toLatin1().constData(),
                                                              provider.displayName().toUtf8().constData()));
    const QString providerIcon = provider.iconName();
    if (providerIcon.isEmpty()) {
        button->setIcon(QIcon::fromTheme("list-add"));
    } else {
        button->setIcon(QIcon::fromTheme(providerIcon));
    }
    button->setProperty("providerName", provider.name());
    button->setToolTip(i18nd(provider.trCatalog().toLatin1().constData(), provider.description().toUtf8().constData()));

    connect(button, SIGNAL(clicked(bool)), SLOT(createAccount()));
    m_form->verticalLayout->insertWidget(0, button);
}

void Create::createAccount()
{
    m_form->progressBar->show();
    m_form->scrollAreaWidgetContents->setEnabled(false);
    QString providerName = sender()->property("providerName").toString();
    qDebug() << "Starting new account dialog for" << providerName;
    CreateAccount *acc = new CreateAccount(providerName, this);

    connect(acc, &CreateAccount::finished, this, [=](KJob *job) {
        m_form->progressBar->hide();
        m_form->scrollAreaWidgetContents->setEnabled(true);
        if (job->error() == KJob::UserDefinedError) {
            QMessageBox::critical(m_parent, i18nc("Messagebox title; meaning 'Unable to finish the action you started'", "Unable to finish"), job->errorText());
        }
        job->deleteLater();
    });

    acc->start();
}
