/*
    Copyright (C) 2014  Martin Klapetek <mklapetek@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "google-contacts-plugin.h"


#include <KSharedConfig>
#include <KConfigGroup>

#include <QTimer>
#include <QStandardPaths>
#include <QDir>
#include <QDBusConnection>
#include <QStringBuilder>
#include <QDebug>
#include <QFile>
#include <QTextStream>

#include <Accounts/Service>
#include <Accounts/Manager>
#include <Accounts/Account>
#include <Accounts/AccountService>
#include <SignOn/IdentityInfo>
#include <SignOn/Identity>

#include "getcredentialsjob.h"
#include "core.h"

#include <kgapi/contacts/contact.h>
#include <kgapi/contacts/contactfetchjob.h>
#include <kgapi/authjob.h>
#include <kgapi/account.h>

#include <KContacts/VCardConverter>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // TODO: print it on stdout too?
    QFile file(QDir::homePath() + "/google-contacts-import.log");

    bool opened = file.open(QIODevice::WriteOnly | QIODevice::Append);
    Q_ASSERT(opened);

    QTextStream out(&file);
    out << QTime::currentTime().toString("hh:mm:ss.zzz ");
    out << context.function << ":" << context.line << " ";

    switch (type) {
        case QtDebugMsg:	out << "DBG"; break;
        case QtWarningMsg:  out << "WRN"; break;
        case QtCriticalMsg: out << "CRT"; break;
        case QtFatalMsg:    out << "FTL"; break;
    }

    out << " " << msg << '\n';
    out.flush();
}

//---------------------------------------------------------------------------------------

class GoogleContactsPlugin::Private {
public:
    Private() {
        retries = 0;
    };

    KGAPI2::AccountPtr account;
    Accounts::AccountId accountId;
    uint retries;
};


//---------------------------------------------------------------------------------------

Q_GLOBAL_STATIC_WITH_ARGS(QString, vcardsLocation, (QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + ("/kpeoplevcard")))

GoogleContactsPlugin::GoogleContactsPlugin(QObject *parent)
    : KAccountsDPlugin(parent),
      d(new Private())
{
    qInstallMessageHandler(myMessageOutput);
}

GoogleContactsPlugin::~GoogleContactsPlugin()
{
}

void GoogleContactsPlugin::onAccountCreated(const Accounts::AccountId accountId, const Accounts::ServiceList &serviceList)
{
    Q_UNUSED(serviceList);
    qDebug() << "New account created";
    d->accountId = accountId;

    Accounts::Account *acc = Accounts::Account::fromId(KAccounts::accountsManager(), accountId);
    if (acc && acc->providerName() == QLatin1String("google")) {
        GetCredentialsJob *job = new GetCredentialsJob(accountId, QStringLiteral("oauth2"), QStringLiteral("web_server"), this);
        connect(job, SIGNAL(finished(KJob*)), SLOT(onCredentialsReceived(KJob*)));
        job->start();
    } else {
        qWarning() << "Account is either invalid or not a google account (account is" << (acc ? acc->providerName() : "");
    }
}

void GoogleContactsPlugin::onCredentialsReceived(KJob *kjob)
{
    qDebug() << "Credentials retrieved, setting up google auth";
    GetCredentialsJob *job = qobject_cast< GetCredentialsJob* >(kjob);
    QVariantMap credentialsData = job->credentialsData();

    d->account = KGAPI2::AccountPtr(new KGAPI2::Account(credentialsData["AccountUsername"].toString(), credentialsData["AccessToken"].toString()));
    d->account->setScopes(QList<QUrl>() << KGAPI2::Account::contactsScopeUrl());

    KGAPI2::ContactFetchJob *fetchJob = new KGAPI2::ContactFetchJob(d->account, this);
    connect(fetchJob, SIGNAL(finished(KGAPI2::Job*)),
            this, SLOT(slotFetchJobFinished(KGAPI2::Job*)));
}

void GoogleContactsPlugin::slotFetchJobFinished(KGAPI2::Job *job)
{
    KGAPI2::ContactFetchJob *fetchJob = qobject_cast<KGAPI2::ContactFetchJob*>(job);
    Q_ASSERT(fetchJob);
    fetchJob->deleteLater();

    if (fetchJob->error() != KGAPI2::NoError) {
        qDebug() << "Error while fetching contacts:" << fetchJob->errorString();
        return;
    }

    /* Get all items the job has retrieved */
    const KGAPI2::ObjectsList objects = fetchJob->items();

    if (objects.size() == 0) {
        if (d->retries < 3) {
            QTimer::singleShot(3000, this, [=]{onAccountCreated(d->accountId, Accounts::ServiceList());});
            d->retries++;
        } else {
            qWarning() << "Retrieved 0 contacts 3 times, giving up";
            d->retries = 0;
        }
        return;
    }

    KContacts::VCardConverter exporter;

    const QString vcardsPath = *vcardsLocation + "/kaccounts_" + QString::number(d->accountId);

    qDebug() << "All set, starting vcards import into" << vcardsPath;

    QDir vcardsDir(vcardsPath);
    vcardsDir.mkpath(vcardsPath);

    Q_FOREACH (const KGAPI2::ObjectPtr &object, objects) {
        const KGAPI2::ContactPtr contact = object.dynamicCast<KGAPI2::Contact>();
        QStringList splits = contact->uid().split("/");
        QFile file(vcardsPath + "/" + splits.last() + ".vcard");
        qDebug() << "Writing to location" << file.fileName();

        bool opened = file.open(QIODevice::WriteOnly | QIODevice::Append);
        Q_ASSERT(opened);

        QTextStream out(&file);
        out << exporter.createVCard(*(contact.data()));
        //qDebug() << ">>>>>>>>>>" << contact->name() << (contact->phoneNumbers().isEmpty() ? "" : contact->phoneNumbers().first().number()) << (contact->emails().isEmpty() ? "" : contact->emails().first()) << contact->uid();
        out.flush();
    }
}


void GoogleContactsPlugin::onAccountRemoved(const Accounts::AccountId accountId)
{
    QDir vcardsDir(*vcardsLocation + "/kaccounts_" + QString::number(accountId));

    if (vcardsDir.exists()) {
        if (vcardsDir.removeRecursively()) {
            qDebug() << "VCards from account" << accountId << "removed successfully";
        } else {
            qWarning() << "Failed to remove VCards for account" << accountId;
        }
    }
}

void GoogleContactsPlugin::onServiceEnabled(const Accounts::AccountId accountId, const Accounts::Service &service)
{
    Q_UNUSED(accountId);
    Q_UNUSED(service);
}

void GoogleContactsPlugin::onServiceDisabled(const Accounts::AccountId accountId, const Accounts::Service &service)
{
    Q_UNUSED(accountId);
    Q_UNUSED(service);
}
