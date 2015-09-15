/*
 * Martin Klapetek (C) 2015 <mklapetek@kde.org>
 * This file is based on buteo-sync-plugin-carddav by Jolla Ltd.
 *
 * Copyright (C) 2014 Jolla Ltd. and/or its subsidiary(-ies).
 *
 * Contributors: Chris Adams <chris.adams@jolla.com>
 *
 * This program/library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program/library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program/library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "syncer_p.h"
#include "carddav_p.h"
#include "auth_p.h"

// #include <twowaycontactsyncadapter_impl.h>
// #include <qtcontacts-extensions_manager_impl.h>

#include <QtCore/QDateTime>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QFile>
#include <QtCore/QByteArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

// #include <QtContacts/QContact>
// #include <QtContacts/QContactManager>
// #include <QtContacts/QContactGuid>
// #include <QtContacts/QContactSyncTarget>
// #include <QtContacts/QContactDetailFilter>
// #include <QtContacts/QContactIntersectionFilter>
// #include <QtContacts/QContactSyncTarget>

#include <Accounts/Manager>
#include <Accounts/Account>

#include <KContacts/VCardConverter>

#include <QCoreApplication>

// #include <SyncProfile.h>
// #include <LogMacros.h>

Q_GLOBAL_STATIC_WITH_ARGS(QString, vcardsLocation, (QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + ("/kpeoplevcard")))

#define CARDDAV_CONTACTS_SYNCTARGET QLatin1String("carddav")
static const int HTTP_UNAUTHORIZED_ACCESS = 401;

Syncer::Syncer(QObject *parent/*, Buteo::SyncProfile *syncProfile*/)
    : QObject(parent) //, QtContactsSqliteExtensions::TwoWayContactSyncAdapter(CARDDAV_CONTACTS_SYNCTARGET)
//     , m_syncProfile(syncProfile)
    , m_cardDav(0)
    , m_auth(0)
    , m_syncAborted(false)
    , m_syncError(false)
    , m_accountId(0)
    , m_ignoreSslErrors(false)
{
}

Syncer::~Syncer()
{
    delete m_auth;
    delete m_cardDav;
}

bool Syncer::testAccountProvenance(const KContacts::Addressee &contact, const QString &accountId)
{
    return contact.uid().startsWith(QStringLiteral("%1:").arg(accountId));
}

void Syncer::abortSync()
{
    m_syncAborted = true;
}

void Syncer::startSync(int accountId)
{
    Q_ASSERT(accountId != 0);
    m_accountId = accountId;
    m_auth = new Auth(this);
    connect(m_auth, SIGNAL(signInCompleted(QString,QString,QString,QString,QString,bool)),
            this, SLOT(sync(QString,QString,QString,QString,QString,bool)));
    connect(m_auth, SIGNAL(signInError()),
            this, SLOT(signInError()));
    qDebug() << "starting carddav sync with account" << m_accountId;
    m_auth->signIn(accountId);
}

void Syncer::signInError()
{
    emit syncFailed();
}

void Syncer::sync(const QString &serverUrl, const QString &addressbookPath, const QString &username, const QString &password, const QString &accessToken, bool ignoreSslErrors)
{
    m_serverUrl = serverUrl;
    m_addressbookPath = addressbookPath;
    m_username = username;
    m_password = password;
    m_accessToken = accessToken;
    m_ignoreSslErrors = ignoreSslErrors;

    QDateTime remoteSince;
//     if (!initSyncAdapter(QString::number(m_accountId))
//             || !readSyncStateData(&remoteSince, QString::number(m_accountId))
//             || !readExtraStateData(m_accountId)) {
//         qWarning() << "unable to init carddav sync for account" << m_accountId;
//         cardDavError();
//         return;
//     }

    qDebug() << "Sync adapter initialised, determining remote changes since" << remoteSince.toString(Qt::ISODate) << "for account" << m_accountId;
    determineRemoteChanges(remoteSince, QString::number(m_accountId));
}

void Syncer::determineRemoteChanges(const QDateTime &, const QString &)
{
    m_cardDav = m_username.isEmpty()
              ? new CardDav(this, m_serverUrl, m_addressbookPath, m_accessToken)
              : new CardDav(this, m_serverUrl, m_addressbookPath, m_username, m_password);
    connect(m_cardDav, SIGNAL(remoteChanges(QList<KContacts::Addressee>,QList<KContacts::Addressee>,QList<KContacts::Addressee>)),
            this, SLOT(continueSync(QList<KContacts::Addressee>,QList<KContacts::Addressee>,QList<KContacts::Addressee>)));
    connect(m_cardDav, SIGNAL(upsyncCompleted()),
            this, SLOT(syncFinished()));
    connect(m_cardDav, SIGNAL(error(int)),
            this, SLOT(cardDavError(int)));
    m_cardDav->determineRemoteAMR();
}

void Syncer::continueSync(const QList<KContacts::Addressee> &added, const QList<KContacts::Addressee> &modified, const QList<KContacts::Addressee> &removed)
{
    if (m_syncAborted || m_syncError) {
        qWarning() << "sync error or aborted";
        cardDavError();
        return;
    }

    // store the remote changes locally
    QList<KContacts::Addressee> addMod = added+modified, del = removed;
    qDebug() << "storing remote changes to local device: AMR:"
             << added.count() << modified.count() << removed.count()
             << "for account:" << m_accountId;

    KContacts::VCardConverter exporter;

    const QString vcardsPath = *vcardsLocation + "/kaccounts_" + QString::number(m_accountId);

    qDebug() << "All set, starting vcards import into" << vcardsPath;

    QDir vcardsDir(vcardsPath);
    vcardsDir.mkpath(vcardsPath);

    Q_FOREACH (const KContacts::Addressee &contact, addMod) {
        QStringList splits = contact.uid().split("/");
        QFile file(vcardsPath + "/" + splits.last() + ".vcard");
        qDebug() << "Writing to location" << file.fileName();

        bool opened = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        Q_ASSERT(opened);

        QTextStream out(&file);
        out << exporter.createVCard(contact);
//         qDebug() << ">>>>>>>>>>" << contact.name() << (contact.phoneNumbers().isEmpty() ? "" : contact.phoneNumbers().first().number()) << (contact.emails().isEmpty() ? "" : contact.emails().first()) << contact.uid();
        out.flush();
    }

//     if (!storeRemoteChanges(del, &addMod, QString::number(m_accountId))) {
//         qWarning() << "unable to store remote changes for account" << m_accountId;
//         cardDavError();
//         return;
//     }
//
//     // now update our id mapping in case anything changed.
//     // this is necessary especially for added contacts, which previously had no id.
//     Q_FOREACH (const KContacts::Addressee &c, addMod) {
//         if (c.id().isNull()) {
//             qWarning() << "no contact id specified for contact with guid"
//                        << c.detail<QContactGuid>().guid() << "from account" << m_accountId;
//             cardDavError();
//             return;
//         } else {
//             m_contactIds.insert(c.detail<QContactGuid>().guid(), c.id().toString());
//         }
//     }
//
//     // continue with the upsync half of the sync process.
//     QDateTime localSince;
//     QList<KContacts::Addressee> locallyAdded, locallyModified, locallyDeleted;
//     // Note: we may still upsync these ignorable details+fields, just don't look at them during delta detection.
//     // We need to do this, otherwise there can be infinite loops caused due to spurious differences between the
//     // in-memory version (QContact) and the exportable version (vCard) resulting in ETag updates server-side.
//     // The downside is that changes to these details will not be upsynced unless another change also occurs.
//     QSet<QContactDetail::DetailType> ignorableDetailTypes = getDefaultIgnorableDetailTypes();
//     ignorableDetailTypes.insert(QContactDetail::TypeFavorite); // ignore differences in X-FAVORITE field when detecting delta.
//     ignorableDetailTypes.insert(QContactDetail::TypeAvatar);   // ignore differences in PHOTO field when detecting delta.
//     QHash<QContactDetail::DetailType, QSet<int> > ignorableDetailFields = getDefaultIgnorableDetailFields();
//     ignorableDetailFields[QContactDetail::TypePhoneNumber] << QContactPhoneNumber::FieldSubTypes; // and TEL number subtypes
//     ignorableDetailFields[QContactDetail::TypeUrl] << QContactUrl::FieldSubType;                  // and URL subtype
//     if (!determineLocalChanges(&localSince, &locallyAdded, &locallyModified, &locallyDeleted,
//                                QString::number(m_accountId), ignorableDetailTypes, ignorableDetailFields)) {
//         qWarning() << "unable to determine local changes for account" << m_accountId;
//         cardDavError();
//         return;
//     }
//
//     if (!m_syncProfile || m_syncProfile->syncDirection() != Buteo::SyncProfile::SYNC_DIRECTION_FROM_REMOTE) {
//         upsyncLocalChanges(localSince, locallyAdded, locallyModified, locallyDeleted, QString::number(m_accountId));
//     } else {
//         qDebug() << "skipping upsync due to sync profile direction setting";
        syncFinished();
//     }
}

void Syncer::upsyncLocalChanges(const QDateTime &localSince,
                                const QList<KContacts::Addressee> &locallyAdded,
                                const QList<KContacts::Addressee> &locallyModified,
                                const QList<KContacts::Addressee> &locallyDeleted,
                                const QString &)
{
    qDebug() << "upsyncing local changes to remote server: AMR:"
             << locallyAdded.count() << locallyModified.count() << locallyDeleted.count()
             << "for account:" << m_accountId << "since:" << localSince;

//     // segment the changes according to the addressbook the contacts are from
//     QSet<QString> modifiedAddressbookUrls;
//     QMap<QString, QList<KContacts::Addressee> > added;
//     QMap<QString, QList<KContacts::Addressee> > modified;
//     QMap<QString, QList<KContacts::Addressee> > deleted;
//
//     QString addedContactsAddressbook = m_defaultAddressbook;
//     if (addedContactsAddressbook.isEmpty()) {
//         addedContactsAddressbook = m_addressbookCtags.keys().size()
//                                  ? m_addressbookCtags.keys().first()
//                                  : QString();
//     }
//     if (addedContactsAddressbook.isEmpty()) {
//         addedContactsAddressbook = m_addressbookSyncTokens.keys().size()
//                                  ? m_addressbookSyncTokens.keys().first()
//                                  : QString();
//     }
//     if (addedContactsAddressbook.isEmpty()) {
//         qWarning() << "no known addressbooks, failing";
//         cardDavError();
//         return;
//     }
//
//     Q_FOREACH (const QContact &a, locallyAdded) {
//         added[addedContactsAddressbook].append(a);
//         modifiedAddressbookUrls.insert(addedContactsAddressbook);
//     }
//     Q_FOREACH (const QContact &m, locallyModified) {
//         Q_FOREACH (const QString &addressbookUrl, m_addressbookContactGuids.keys()) {
//             if (m_addressbookContactGuids[addressbookUrl].contains(m.detail<QContactGuid>().guid())) {
//                 modified[addressbookUrl].append(m);
//                 modifiedAddressbookUrls.insert(addressbookUrl);
//             }
//         }
//     }
//     Q_FOREACH (const QContact &d, locallyDeleted) {
//         Q_FOREACH (const QString &addressbookUrl, m_addressbookContactGuids.keys()) {
//             if (m_addressbookContactGuids[addressbookUrl].contains(d.detail<QContactGuid>().guid())) {
//                 deleted[addressbookUrl].append(d);
//                 modifiedAddressbookUrls.insert(addressbookUrl);
//             }
//         }
//     }
//
//     // now upsync the changes for each addressbook
//     if (modifiedAddressbookUrls.size()) {
//         Q_FOREACH (const QString &addressbookUrl, modifiedAddressbookUrls) {
//             m_cardDav->upsyncUpdates(addressbookUrl,
//                                      added[addressbookUrl],
//                                      modified[addressbookUrl],
//                                      deleted[addressbookUrl]);
//         }
//     } else {
        // nothing to upsync.
        syncFinished();
//     }
}

void Syncer::syncFinished()
{
    // finished upsync.  Just need to store our state data and we're done.
    qDebug() << "about to store sync state data";
    if (!storeExtraStateData(m_accountId)) { // || !storeSyncStateData(QString::number(m_accountId))) {
        qWarning() << "unable to finalise sync state";
        cardDavError(); // actually in this case we have already stored stuff to local and server...?
        return;
    }

    // Success.
    qDebug() << "carddav sync with account" << m_accountId << "finished successfully!";
    emit syncSucceeded();
}

void Syncer::cardDavError(int errorCode)
{
    qWarning() << "CardDAV sync finished with error:" << errorCode <<
                "purging state data for account:" << m_accountId;
    m_syncError = true;
    if (errorCode == HTTP_UNAUTHORIZED_ACCESS) {
        m_auth->setCredentialsNeedUpdate(m_accountId);
    }
    purgeExtraStateData(m_accountId);
//     purgeSyncStateData(QString::number(m_accountId));
    emit syncFailed();
}

void Syncer::purgeAccount(int accountId)
{
//     QContactDetailFilter syncTargetFilter;
//     syncTargetFilter.setDetailType(QContactDetail::TypeSyncTarget, QContactSyncTarget::FieldSyncTarget);
//     syncTargetFilter.setValue(CARDDAV_CONTACTS_SYNCTARGET);
//     QContactDetailFilter guidFilter;
//     guidFilter.setDetailType(QContactDetail::TypeGuid, QContactGuid::FieldGuid);
//     guidFilter.setValue(QStringLiteral("%1:").arg(accountId));
//     guidFilter.setMatchFlags(QContactDetailFilter::MatchStartsWith);
//     QList<QContactId> contactsToRemove = m_contactManager.contactIds(syncTargetFilter & guidFilter);
//
//     // now write the changes to the database.
//     bool success = true;
//     if (contactsToRemove.size()) {
//         success = m_contactManager.removeContacts(contactsToRemove);
//         if (!success) {
//             qWarning() << "Failed to remove stale contacts during purge of account" << accountId
//                        << ":" << m_contactManager.error();
//         }
//     }
//
//     // ensure we remove the OOB data for the account.
//     // We can't rely on d->m_stateData[QString::number(accountId)].m_oobScope containing the
//     // correct value, as the purge codepath can be called from cleanUp() on account
//     // removal, during which no cached state data exists.
//     // Also, it may be called for an account which was previously removed but for which
//     // artifacts still remain (eg, if msyncd wasn't running at the time that the account
//     // was removed, due to a crash, etc) - in which case the cached value would be wrong.
//     QString oobScope = QStringLiteral("%1-%2").arg(CARDDAV_CONTACTS_SYNCTARGET).arg(accountId);
//     if (!d->m_engine->removeOOB(oobScope)) {
//         success = false;
//         qWarning() << "Error occurred while purging OOB data for removed CardDAV account" << accountId;
//     }
//
//     if (success) {
//         qDebug() << "Purged account" << accountId
//                  << "and successfully removed" << contactsToRemove.size() << "contacts";
//     }
}

// this function must be called directly after readSyncStateData()
bool Syncer::readExtraStateData(int accountId)
{
    QMap<QString, QVariant> values;
    QStringList keys;
    keys << QStringLiteral("addressbookContactGuids")
         << QStringLiteral("addressbookCtags")
         << QStringLiteral("addressbookSyncTokens")
         << QStringLiteral("contactUids")
         << QStringLiteral("contactUris")
         << QStringLiteral("contactEtags")
         << QStringLiteral("contactIds")
         << QStringLiteral("contactUnsupportedProperties");
//     if (!d->m_engine->fetchOOB(d->m_stateData[QString::number(accountId)].m_oobScope, keys, &values)) {
//         qWarning() << "failed to read extra data for carddav account" << accountId;
//         d->clear(QString::number(accountId));
//         return false;
//     }

    // m_addressbookContactGuids
    QVariant acgValue = values.value(QStringLiteral("addressbookContactGuids"));
    QByteArray acgValueBA = acgValue.toByteArray();
    QJsonObject acgJsonObj = QJsonDocument::fromBinaryData(acgValueBA).object();
    QStringList addressbookUrls = acgJsonObj.keys();
    QMap<QString, QStringList> addressbookUrlToContactGuids;
    foreach (const QString &url, addressbookUrls) {
        QVariantList contactGuidsVL = acgJsonObj.value(url).toArray().toVariantList();
        QStringList contactGuids;
        foreach (const QVariant &v, contactGuidsVL) {
            if (!v.toString().isEmpty()) {
                contactGuids.append(v.toString());
            }
        }

        addressbookUrlToContactGuids.insert(url, contactGuids);
    }
    m_addressbookContactGuids = addressbookUrlToContactGuids;

    // m_addressbookCtags
    QVariant acValue = values.value(QStringLiteral("addressbookCtags"));
    QByteArray acValueBA = acValue.toByteArray();
    QJsonObject acJsonObj = QJsonDocument::fromBinaryData(acValueBA).object();
    addressbookUrls = acJsonObj.keys();
    QMap<QString, QString> addressbookUrlToCtag;
    foreach (const QString &url, addressbookUrls) {
        addressbookUrlToCtag.insert(url, acJsonObj.value(url).toString());
    }
    m_addressbookCtags = addressbookUrlToCtag;

    // m_addressbookSyncTokens
    QVariant asValue = values.value(QStringLiteral("addressbookSyncTokens"));
    QByteArray asValueBA = asValue.toByteArray();
    QJsonObject asJsonObj = QJsonDocument::fromBinaryData(asValueBA).object();
    addressbookUrls = asJsonObj.keys();
    QMap<QString, QString> addressbookUrlToSyncToken;
    foreach (const QString &url, addressbookUrls) {
        addressbookUrlToSyncToken.insert(url, asJsonObj.value(url).toString());
    }
    m_addressbookSyncTokens = addressbookUrlToSyncToken;

    // m_contactUids
    QVariant cuiValue = values.value(QStringLiteral("contactUids"));
    QByteArray cuiValueBA = cuiValue.toByteArray();
    QJsonObject cuiJsonObj = QJsonDocument::fromBinaryData(cuiValueBA).object();
    QStringList contactGuids = cuiJsonObj.keys();
    QMap<QString, QString> guidToContactUid;
    foreach (const QString &guid, contactGuids) {
        guidToContactUid.insert(guid, cuiJsonObj.value(guid).toString());
    }
    m_contactUids = guidToContactUid;

    // m_contactUris
    QVariant cuValue = values.value(QStringLiteral("contactUris"));
    QByteArray cuValueBA = cuValue.toByteArray();
    QJsonObject cuJsonObj = QJsonDocument::fromBinaryData(cuValueBA).object();
    contactGuids = cuJsonObj.keys();
    QMap<QString, QString> guidToContactUri;
    foreach (const QString &guid, contactGuids) {
        guidToContactUri.insert(guid, cuJsonObj.value(guid).toString());
    }
    m_contactUris = guidToContactUri;

    // m_contactEtags
    QVariant ceValue = values.value(QStringLiteral("contactEtags"));
    QByteArray ceValueBA = ceValue.toByteArray();
    QJsonObject ceJsonObj = QJsonDocument::fromBinaryData(ceValueBA).object();
    contactGuids = ceJsonObj.keys();
    QMap<QString, QString> guidToContactEtag;
    foreach (const QString &guid, contactGuids) {
        guidToContactEtag.insert(guid, ceJsonObj.value(guid).toString());
    }
    m_contactEtags = guidToContactEtag;

    // m_contactIds
    QVariant ciValue = values.value(QStringLiteral("contactIds"));
    QByteArray ciValueBA = ciValue.toByteArray();
    QJsonObject ciJsonObj = QJsonDocument::fromBinaryData(ciValueBA).object();
    contactGuids = ciJsonObj.keys();
    QMap<QString, QString> guidToContactId;
    foreach (const QString &guid, contactGuids) {
        guidToContactId.insert(guid, ciJsonObj.value(guid).toString());
    }
    m_contactIds = guidToContactId;

    // m_contactUnsupportedProperties
    QVariant cupValue = values.value(QStringLiteral("contactUnsupportedProperties"));
    QByteArray cupValueBA = cupValue.toByteArray();
    QJsonObject cupJsonObj = QJsonDocument::fromBinaryData(cupValueBA).object();
    contactGuids = cupJsonObj.keys();
    QMap<QString, QStringList> contactGuidToUnsupportedProperties;
    foreach (const QString &guid, contactGuids) {
        QVariantList unsupportedPropertiesVL = cupJsonObj.value(guid).toArray().toVariantList();
        QStringList unsupportedProperties;
        foreach (const QVariant &v, unsupportedPropertiesVL) {
            if (!v.toString().isEmpty()) {
                unsupportedProperties.append(v.toString());
            }
        }

        contactGuidToUnsupportedProperties.insert(guid, unsupportedProperties);
    }
    m_contactUnsupportedProperties = contactGuidToUnsupportedProperties;

    // Finally, if we're doing a "clean sync" we should pre-populate our prevRemote
    // list with the current state of the local database.
    // This is to avoid clean-syncs causing contact duplication.
//     if (!d->m_stateData[QString::number(m_accountId)].m_localSince.isValid()) {
//         QDateTime maxTimestamp;
//         QList<KContacts::Addressee> existingContacts;
// //         QContactManager::Error error = QContactManager::NoError;
//         if (!d->m_engine->fetchSyncContacts(CARDDAV_CONTACTS_SYNCTARGET,
//                                             QDateTime(),
//                                             QList<QContactId>(),
//                                             &existingContacts,
//                                             0,
//                                             0,
//                                             &maxTimestamp,
//                                             &error)) {
//             qWarning() << "failed to fetch pre-existing contacts for account" << m_accountId;
//             d->clear(QString::number(accountId));
//             return false;
//         }
//
//         // filter out any which don't come from this account.
//         QList<KContacts::Addressee> prevRemote;
//         QList<QContactId> exportedIds;
//         foreach (const QContact &c, existingContacts) {
//             if (c.detail<QContactGuid>().guid().startsWith(QStringLiteral("%1:").arg(accountId))) {
//                 prevRemote.append(c);
//                 exportedIds.append(c.id());
//                 m_contactIds.insert(c.detail<QContactGuid>().guid(), c.id().toString());
//             }
//         }
//
//         // set our state data.
//         d->m_stateData[QString::number(accountId)].m_prevRemote = prevRemote;
//         d->m_stateData[QString::number(accountId)].m_exportedIds = exportedIds;
//     }

    // done.
    return true;
}

// this function must be called directly before storeSyncStateData()
bool Syncer::storeExtraStateData(int accountId)
{
    // m_addressbookContactGuids
    QJsonObject acgJsonObj;
    for (QMap<QString, QStringList>::const_iterator it = m_addressbookContactGuids.constBegin();
            it != m_addressbookContactGuids.constEnd(); ++it) {
        acgJsonObj.insert(it.key(), QJsonValue(QJsonArray::fromStringList(it.value())));
    }
    QJsonDocument acgJsonDoc(acgJsonObj);
    QVariant acgValue(acgJsonDoc.toBinaryData());

    // m_addressbookCtags
    QJsonObject acJsonObj;
    for (QMap<QString, QString>::const_iterator it = m_addressbookCtags.constBegin();
            it != m_addressbookCtags.constEnd(); ++it) {
        acJsonObj.insert(it.key(), QJsonValue(it.value()));
    }
    QJsonDocument acJsonDoc(acJsonObj);
    QVariant acValue(acJsonDoc.toBinaryData());

    // m_addressbookSyncTokens
    QJsonObject asJsonObj;
    for (QMap<QString, QString>::const_iterator it = m_addressbookSyncTokens.constBegin();
            it != m_addressbookSyncTokens.constEnd(); ++it) {
        asJsonObj.insert(it.key(), QJsonValue(it.value()));
    }
    QJsonDocument asJsonDoc(asJsonObj);
    QVariant asValue(asJsonDoc.toBinaryData());

    // m_contactUids
    QJsonObject cuiJsonObj;
    for (QMap<QString, QString>::const_iterator it = m_contactUids.constBegin();
            it != m_contactUids.constEnd(); ++it) {
        cuiJsonObj.insert(it.key(), QJsonValue(it.value()));
    }
    QJsonDocument cuiJsonDoc(cuiJsonObj);
    QVariant cuiValue(cuiJsonDoc.toBinaryData());

    // m_contactUris
    QJsonObject cuJsonObj;
    for (QMap<QString, QString>::const_iterator it = m_contactUris.constBegin();
            it != m_contactUris.constEnd(); ++it) {
        cuJsonObj.insert(it.key(), QJsonValue(it.value()));
    }
    QJsonDocument cuJsonDoc(cuJsonObj);
    QVariant cuValue(cuJsonDoc.toBinaryData());

    // m_contactEtags
    QJsonObject ceJsonObj;
    for (QMap<QString, QString>::const_iterator it = m_contactEtags.constBegin();
            it != m_contactEtags.constEnd(); ++it) {
        ceJsonObj.insert(it.key(), QJsonValue(it.value()));
    }
    QJsonDocument ceJsonDoc(ceJsonObj);
    QVariant ceValue(ceJsonDoc.toBinaryData());

    // m_contactIds
    QJsonObject ciJsonObj;
    for (QMap<QString, QString>::const_iterator it = m_contactIds.constBegin();
            it != m_contactIds.constEnd(); ++it) {
        ciJsonObj.insert(it.key(), QJsonValue(it.value()));
    }
    QJsonDocument ciJsonDoc(ciJsonObj);
    QVariant ciValue(ciJsonDoc.toBinaryData());

    // m_contactUnsupportedProperties
    QJsonObject cupJsonObj;
    for (QMap<QString, QStringList>::const_iterator it = m_contactUnsupportedProperties.constBegin();
            it != m_contactUnsupportedProperties.constEnd(); ++it) {
        cupJsonObj.insert(it.key(), QJsonValue(QJsonArray::fromStringList(it.value())));
    }
    QJsonDocument cupJsonDoc(cupJsonObj);
    QVariant cupValue(cupJsonDoc.toBinaryData());

    // store to OOB
    QMap<QString, QVariant> values;
    values.insert("addressbookContactGuids", acgValue);
    values.insert("addressbookCtags", acValue);
    values.insert("addressbookSyncTokens", asValue);
    values.insert("contactUids", cuiValue);
    values.insert("contactUris", cuValue);
    values.insert("contactEtags", ceValue);
    values.insert("contactIds", ciValue);
    values.insert("contactUnsupportedProperties", cupValue);
//     if (!d->m_engine->storeOOB(d->m_stateData[QString::number(accountId)].m_oobScope, values)) {
//         qWarning() << "failed to store extra state data for carddav account" << accountId;
//         d->clear(QString::number(accountId));
//         return false;
//     }

    return true;
}

// this function must be called directly before purgeSyncStateData()
bool Syncer::purgeExtraStateData(int accountId)
{
    QStringList purgeKeys;
    purgeKeys << QStringLiteral("addressbookContactGuids") << QStringLiteral("addressbookCtags");
    purgeKeys << QStringLiteral("addressbookSyncTokens") << QStringLiteral("contactUids");
    purgeKeys << QStringLiteral("contactUris") << QStringLiteral("contactEtags");
    purgeKeys << QStringLiteral("contactIds") << QStringLiteral("contactUnsupportedProperties");
//     if (!d->m_engine->removeOOB(d->m_stateData[QString::number(accountId)].m_oobScope, purgeKeys)) {
//         qWarning() << "failed to remove extra state data for carddav account" << accountId;
//         return false;
//     }
    return true;
}
