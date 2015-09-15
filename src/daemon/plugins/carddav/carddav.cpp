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

#include "carddav_p.h"
#include "syncer_p.h"

#include <QRegularExpression>
#include <QUuid>
#include <QByteArray>
#include <QBuffer>
#include <QTimer>
#include <QDebug>

#include <KContacts/VCardConverter>
#include <KContacts/Addressee>

// #include <seasidepropertyhandler.h>

// #include <qtcontacts-extensions.h>

namespace {
    void debugDumpData(const QString &data)
    {
        QString dbgout;
        Q_FOREACH (const QChar &c, data) {
            if (c == '\r' || c == '\n') {
                if (!dbgout.isEmpty()) {
                    qDebug() << dbgout;
                    dbgout.clear();
                }
            } else {
                dbgout += c;
            }
        }
        if (!dbgout.isEmpty()) {
            qDebug() << dbgout;
        }
    }
}

CardDav::CardDav(Syncer *parent,
                 const QString &serverUrl,
                 const QString &addressbookPath,
                 const QString &username,
                 const QString &password)
    : QObject(parent)
    , q(parent)
    , m_request(new RequestGenerator(q, username, password))
    , m_parser(new ReplyParser(q))
    , m_serverUrl(serverUrl)
    , m_addressbookPath(addressbookPath)
    , m_discoveryStage(CardDav::DiscoveryStarted)
    , m_downsyncRequests(0)
    , m_upsyncRequests(0)
{
}

CardDav::CardDav(Syncer *parent,
                 const QString &serverUrl,
                 const QString &addressbookPath,
                 const QString &accessToken)
    : QObject(parent)
    , q(parent)
    , m_request(new RequestGenerator(q, accessToken))
    , m_parser(new ReplyParser(q))
    , m_serverUrl(serverUrl)
    , m_addressbookPath(addressbookPath)
    , m_discoveryStage(CardDav::DiscoveryStarted)
    , m_downsyncRequests(0)
    , m_upsyncRequests(0)
{
}

CardDav::~CardDav()
{
    delete m_request;
    delete m_parser;
}

void CardDav::errorOccurred(int httpError)
{
    emit error(httpError);
}

void CardDav::determineRemoteAMR()
{
    if (m_addressbookPath.isEmpty()) {
        // The CardDAV sequence for determining the A/M/R delta is:
        // a)  fetch user information from the principal URL
        // b)  fetch addressbooks home url
        // c)  fetch addressbook information
        // d)  for each addressbook, either:
        //     i)  perform immediate delta sync (if webdav-sync enabled) OR
        //     ii) fetch etags, manually calculate delta
        // e) fetch full contacts for delta.

        // We start by fetching user information.
        fetchUserInformation();
    } else {
        // we can skip to step (c) of the discovery.
        fetchAddressbooksInformation(m_addressbookPath);
    }
}

void CardDav::fetchUserInformation()
{
    qDebug() << "requesting principal urls for user";

    // we need to specify the .well-known/carddav endpoint if it's the first
    // request (so we have not yet been redirected to the correct endpoint)
    // and if the path is empty/unknown.

    /*
        RFC 6764 section 6.5:

        * The client does a "PROPFIND" [RFC4918] request with the
          request URI set to the initial "context path".  The body of
          the request SHOULD include the DAV:current-user-principal
          [RFC5397] property as one of the properties to return.  Note
          that clients MUST properly handle HTTP redirect responses for
          the request.  The server will use the HTTP authentication
          procedure outlined in [RFC2617] or use some other appropriate
          authentication schemes to authenticate the user.

        * When an initial "context path" has not been determined from a
          TXT record, the initial "context path" is taken to be
          "/.well-known/caldav" (for CalDAV) or "/.well-known/carddav"
          (for CardDAV).

        * If the server returns a 404 ("Not Found") HTTP status response
          to the request on the initial "context path", clients MAY try
          repeating the request on the "root" URI "/" or prompt the user
          for a suitable path.
    */

    QUrl serverUrl(m_serverUrl);
    QString wellKnownUrl = QStringLiteral("%1://%2/.well-known/carddav").arg(serverUrl.scheme()).arg(serverUrl.host());
    bool firstRequest = m_discoveryStage == CardDav::DiscoveryStarted;
    m_serverUrl = firstRequest && (serverUrl.path().isEmpty() || serverUrl.path() == QStringLiteral("/"))
                ? wellKnownUrl
                : m_serverUrl;
    QNetworkReply *reply = m_request->currentUserInformation(m_serverUrl);
    if (!reply) {
        emit error();
        return;
    }

    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrorsOccurred(QList<QSslError>)));
    connect(reply, SIGNAL(finished()), this, SLOT(userInformationResponse()));
}

void CardDav::sslErrorsOccurred(const QList<QSslError> &errors)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (q->m_ignoreSslErrors) {
        qDebug() << "ignoring SSL errors due to account policy:" << errors;
        reply->ignoreSslErrors(errors);
    } else {
        qWarning() << "SSL errors occurred, aborting:" << errors;
        errorOccurred(401);
    }
}

void CardDav::userInformationResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        int httpError = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qWarning() << "error:" << reply->error() << "(" << httpError << ") to request" << m_serverUrl;
        debugDumpData(QString::fromUtf8(data));
        QUrl oldServerUrl(m_serverUrl);
        if (m_discoveryStage == CardDav::DiscoveryStarted && (httpError == 404 || httpError == 405)) {
            if (!oldServerUrl.path().endsWith(QStringLiteral(".well-known/carddav"))) {
                // From RFC 6764: If the initial "context path" derived from a TXT record
                // generates HTTP errors when targeted by requests, the client
                // SHOULD repeat its "bootstrapping" procedure using the
                // appropriate ".well-known" URI instead.
                qDebug() << "got HTTP response" << httpError << "to initial discovery request; trying well-known URI";
                m_serverUrl = QStringLiteral("%1://%2/.well-known/carddav").arg(oldServerUrl.scheme()).arg(oldServerUrl.host());
                fetchUserInformation(); // set initial context path to well-known URI.
            } else {
                // From RFC 6764: if the server returns a 404 HTTP status response to the
                // request on the initial context path, clients may try repeating the request
                // on the root URI.
                // We also do this on HTTP 405 in case some implementation is non-spec-conformant.
                qDebug() << "got HTTP response" << httpError << "to well-known request; trying root URI";
                m_discoveryStage = CardDav::DiscoveryTryRoot;
                m_serverUrl = QStringLiteral("%1://%2/").arg(oldServerUrl.scheme()).arg(oldServerUrl.host());
                fetchUserInformation();
            }
            return;
        }
        errorOccurred(httpError);
        return;
    }

    // if the request was to the /.well-known/carddav path, then we need to redirect
    QUrl redir = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!redir.isEmpty()) {
        QUrl orig = reply->url();
        if (orig.path() != redir.path()) {
            if (orig.path().endsWith(QStringLiteral(".well-known/carddav"))) {
                // redirect as required, and change our server URL to point to the redirect URL.
                qDebug() << "redirecting from:" << orig.toString() << "to:" << redir.toString();
                m_serverUrl = QStringLiteral("%1://%2%3").arg(redir.scheme()).arg(redir.host()).arg(redir.path());
                m_discoveryStage = CardDav::DiscoveryRedirected;
                fetchUserInformation();
            } else {
                // possibly unsafe redirect.  for security, assume it's malicious and abort sync.
                qWarning() << "unexpected redirect from:" << orig.toString() << "to:" << redir.toString();
                errorOccurred(301);
            }
        } else {
            // circular redirect, avoid the endless loop by aborting sync.
            qWarning() << "redirect specified is circular:" << redir.toString();
            errorOccurred(301);
        }
        return;
    }

    ReplyParser::ResponseType responseType = ReplyParser::UserPrincipalResponse;
    QString userPath = m_parser->parseUserPrincipal(data, &responseType);
    if (responseType == ReplyParser::UserPrincipalResponse) {
        // the server responded with the expected user principal information.
        if (userPath.isEmpty()) {
            qWarning() << "unable to parse user principal from response";
            emit error();
            return;
        }
        fetchAddressbookUrls(userPath);
    } else if (responseType == ReplyParser::AddressbookInformationResponse) {
        // the server responded with addressbook information instead
        // of user principal information.  Skip the next discovery step.
        QList<ReplyParser::AddressBookInformation> infos = m_parser->parseAddressbookInformation(data);
        if (infos.isEmpty()) {
            qWarning() << "unable to parse addressbook info from user principal response";
            emit error();
            return;
        }
        downsyncAddressbookContent(infos);
    } else {
        qWarning() << "unknown response from user principal request";
        emit error();
    }
}

void CardDav::fetchAddressbookUrls(const QString &userPath)
{
    qDebug() << "requesting addressbook urls for user";
    QNetworkReply *reply = m_request->addressbookUrls(m_serverUrl, userPath);
    if (!reply) {
        emit error();
        return;
    }

    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrorsOccurred(QList<QSslError>)));
    connect(reply, SIGNAL(finished()), this, SLOT(addressbookUrlsResponse()));
}

void CardDav::addressbookUrlsResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        int httpError = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qWarning() << "error:" << reply->error() << "(" << httpError << ")";
        debugDumpData(QString::fromUtf8(data));
        errorOccurred(httpError);
        return;
    }

    QString addressbooksHomePath = m_parser->parseAddressbookHome(data);
    if (addressbooksHomePath.isEmpty()) {
        qWarning() << "unable to parse addressbook home from response";
        emit error();
        return;
    }

    fetchAddressbooksInformation(addressbooksHomePath);
}

void CardDav::fetchAddressbooksInformation(const QString &addressbooksHomePath)
{
    qDebug() << "requesting addressbook sync information" << m_serverUrl << addressbooksHomePath;
    QNetworkReply *reply = m_request->addressbooksInformation(m_serverUrl, addressbooksHomePath);
    if (!reply) {
        emit error();
        return;
    }

    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrorsOccurred(QList<QSslError>)));
    connect(reply, SIGNAL(finished()), this, SLOT(addressbooksInformationResponse()));
}

void CardDav::addressbooksInformationResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        int httpError = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qWarning() << "error:" << reply->error() << "(" << httpError << ")";
        debugDumpData(QString::fromUtf8(data));
        errorOccurred(httpError);
        return;
    }

    QList<ReplyParser::AddressBookInformation> infos = m_parser->parseAddressbookInformation(data);
    if (infos.isEmpty()) {
        qWarning() << "unable to parse addressbook info from response";
        emit error();
        return;
    }

    downsyncAddressbookContent(infos);
}

void CardDav::downsyncAddressbookContent(const QList<ReplyParser::AddressBookInformation> &infos)
{
    // for addressbooks which support sync-token syncing, use that style.
    for (int i = 0; i < infos.size(); ++i) {
        // set a default addressbook if we haven't seen one yet.
        // we will store newly added local contacts to that addressbook.
        if (q->m_defaultAddressbook.isEmpty()) {
            q->m_defaultAddressbook = infos[i].url;
        }

        if (infos[i].syncToken.isEmpty()) {
            // we cannot use sync-token for this addressbook, but instead ctag.
            const QString &existingCtag(q->m_addressbookCtags[infos[i].url]); // from OOB
            if (existingCtag.isEmpty()) {
                // first time sync
                q->m_addressbookCtags[infos[i].url] = infos[i].ctag; // insert
                // now do etag request, the delta will be all remote additions
                fetchContactMetadata(infos[i].url);
            } else if (existingCtag != infos[i].ctag) {
                // changes have occurred since last sync
                q->m_addressbookCtags[infos[i].url] = infos[i].ctag; // update
                // perform etag request and then manually calculate deltas.
                fetchContactMetadata(infos[i].url);
            } else {
                // no changes have occurred in this addressbook since last sync
                qDebug() << "no changes since last sync for"
                         << infos[i].url << "from account" << q->m_accountId;
                m_downsyncRequests += 1;
                QTimer::singleShot(0, this, SLOT(downsyncComplete()));
            }
        } else {
            // the server supports webdav-sync for this addressbook.
            const QString &existingSyncToken(q->m_addressbookSyncTokens[infos[i].url]); // from OOB
            // store the ctag anyway just in case the server has
            // forgotten the syncToken we cached from last time.
            if (!infos[i].ctag.isEmpty()) {
                q->m_addressbookCtags[infos[i].url] = infos[i].ctag;
            }
            // attempt to perform synctoken sync
            if (existingSyncToken.isEmpty()) {
                // first time sync
                q->m_addressbookSyncTokens[infos[i].url] = infos[i].syncToken; // insert
                // perform slow sync / full report
                fetchContactMetadata(infos[i].url);
            } else if (existingSyncToken != infos[i].syncToken) {
                // changes have occurred since last sync.
                q->m_addressbookSyncTokens[infos[i].url] = infos[i].syncToken; // update
                // perform immediate delta sync, by passing the old sync token to the server.
                fetchImmediateDelta(infos[i].url, existingSyncToken);
            } else {
                // no changes have occurred in this addressbook since last sync
                qDebug() << "no changes since last sync for"
                         << infos[i].url << "from account" << q->m_accountId;
                m_downsyncRequests += 1;
                QTimer::singleShot(0, this, SLOT(downsyncComplete()));
            }
        }
    }
}

void CardDav::fetchImmediateDelta(const QString &addressbookUrl, const QString &syncToken)
{
    qDebug() << "requesting immediate delta for addressbook" << addressbookUrl
             << "with sync token" << syncToken;

    QNetworkReply *reply = m_request->syncTokenDelta(m_serverUrl, addressbookUrl, syncToken);
    if (!reply) {
        emit error();
        return;
    }

    m_downsyncRequests += 1; // when this reaches zero, we've finished all addressbook deltas
    reply->setProperty("addressbookUrl", addressbookUrl);
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrorsOccurred(QList<QSslError>)));
    connect(reply, SIGNAL(finished()), this, SLOT(immediateDeltaResponse()));
}

void CardDav::immediateDeltaResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QString addressbookUrl = reply->property("addressbookUrl").toString();
    QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "error:" << reply->error()
                   << "(" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << ")";
        debugDumpData(QString::fromUtf8(data));
        // The server is allowed to forget the syncToken by the
        // carddav protocol.  Try a full report sync just in case.
        fetchContactMetadata(addressbookUrl);
        return;
    }

    QString newSyncToken;
    QList<ReplyParser::ContactInformation> infos = m_parser->parseSyncTokenDelta(data, &newSyncToken);
    q->m_addressbookSyncTokens[addressbookUrl] = newSyncToken;
    fetchContacts(addressbookUrl, infos);
}

void CardDav::fetchContactMetadata(const QString &addressbookUrl)
{
    qDebug() << "requesting contact metadata for addressbook" << addressbookUrl;
    QNetworkReply *reply = m_request->contactEtags(m_serverUrl, addressbookUrl);
    if (!reply) {
        emit error();
        return;
    }

    m_downsyncRequests += 1; // when this reaches zero, we've finished all addressbook deltas
    reply->setProperty("addressbookUrl", addressbookUrl);
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrorsOccurred(QList<QSslError>)));
    connect(reply, SIGNAL(finished()), this, SLOT(contactMetadataResponse()));
}

void CardDav::contactMetadataResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QString addressbookUrl = reply->property("addressbookUrl").toString();
    QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        int httpError = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qWarning() << "error:" << reply->error() << "(" << httpError << ")";
        debugDumpData(QString::fromUtf8(data));
        errorOccurred(httpError);
        return;
    }

    QList<ReplyParser::ContactInformation> infos = m_parser->parseContactMetadata(data, addressbookUrl);
    fetchContacts(addressbookUrl, infos);
}

void CardDav::fetchContacts(const QString &addressbookUrl, const QList<ReplyParser::ContactInformation> &amrInfo)
{
    qDebug() << "requesting full contact information from addressbook" << addressbookUrl;

    // split into A/M/R request sets
    QStringList contactUris;
    Q_FOREACH (const ReplyParser::ContactInformation &info, amrInfo) {
        if (info.modType == ReplyParser::ContactInformation::Addition) {
            q->m_serverAdditionIndices[addressbookUrl].insert(info.uri, q->m_serverAdditions[addressbookUrl].size());
            q->m_serverAdditions[addressbookUrl].append(info);
            contactUris.append(info.uri);
        } else if (info.modType == ReplyParser::ContactInformation::Modification) {
            q->m_serverModificationIndices[addressbookUrl].insert(info.uri, q->m_serverModifications[addressbookUrl].size());
            q->m_serverModifications[addressbookUrl].append(info);
            contactUris.append(info.uri);
        } else if (info.modType == ReplyParser::ContactInformation::Deletion) {
            q->m_serverDeletions[addressbookUrl].append(info);
        } else {
            qWarning() << "no modification type in info for:" << info.uri;
        }
    }

    qDebug() << "Have calculated AMR:"
             << q->m_serverAdditions[addressbookUrl].size()
             << q->m_serverModifications[addressbookUrl].size()
             << q->m_serverDeletions[addressbookUrl].size()
             << "for addressbook:" << addressbookUrl;

    if (contactUris.isEmpty()) {
        // no additions or modifications to fetch.
        qDebug() << "no further data to fetch";
        contactAddModsComplete(addressbookUrl);
    } else {
        // fetch the full contact data for additions/modifications.
        qDebug() << "fetching vcard data for" << contactUris.size() << "contacts";
        QNetworkReply *reply = m_request->contactMultiget(m_serverUrl, addressbookUrl, contactUris);
        if (!reply) {
            emit error();
            return;
        }

        reply->setProperty("addressbookUrl", addressbookUrl);
        connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrorsOccurred(QList<QSslError>)));
        connect(reply, SIGNAL(finished()), this, SLOT(contactsResponse()));
    }
}

void CardDav::contactsResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QString addressbookUrl = reply->property("addressbookUrl").toString();
    QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        int httpError = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qWarning() <<"error:" << reply->error() << "(" << httpError << ")";
        debugDumpData(QString::fromUtf8(data));
        errorOccurred(httpError);
        return;
    }

    QList<KContacts::Addressee> added;
    QList<KContacts::Addressee> modified;

    // fill out added/modified.  Also keep our addressbookContactGuids state up-to-date.
    // The addMods map is a map from server contact uri to <contact/unsupportedProperties/etag>.
    QMap<QString, ReplyParser::FullContactInformation> addMods = m_parser->parseContactData(data);
    QMap<QString, ReplyParser::FullContactInformation>::const_iterator it = addMods.constBegin();
    for ( ; it != addMods.constEnd(); ++it) {
        if (q->m_serverAdditionIndices[addressbookUrl].contains(it.key())) {
            const QString guid = it.value().contact.uid();
            q->m_serverAdditions[addressbookUrl][q->m_serverAdditionIndices[addressbookUrl].value(it.key())].guid = guid;
            q->m_contactEtags[guid] = it.value().etag;
            q->m_contactUris[guid] = it.key();
            q->m_contactUnsupportedProperties[guid] = it.value().unsupportedProperties;
            // Note: for additions, q->m_contactUids will have been filled out by the reply parser.
            q->m_addressbookContactGuids[addressbookUrl].append(guid);
            // Check to see if this server-side addition is actually just
            // a reported previously-upsynced local-side addition.
            if (q->m_contactIds.contains(guid)) {
                KContacts::Addressee previouslyUpsynced = it.value().contact;
                previouslyUpsynced.setUid(q->m_contactIds[guid]);
                added.append(previouslyUpsynced);
            } else {
                // pure server-side addition.
                added.append(it.value().contact);
            }
        } else if (q->m_serverModificationIndices[addressbookUrl].contains(it.key())) {
            KContacts::Addressee c = it.value().contact;
            const QString guid = c.uid();
            q->m_contactUnsupportedProperties[guid] = it.value().unsupportedProperties;
            q->m_contactEtags[guid] = it.value().etag;
            if (!q->m_contactIds.contains(guid)) {
                qWarning() << "modified contact has no id";
            } else {
                c.setUid(q->m_contactIds[guid]);
            }
            modified.append(c);
        } else {
            qWarning() << "ignoring unknown addition/modification:" << it.key();
        }
    }

    // coalesce the added/modified contacts from this addressbook into the complete AMR
    m_remoteAdditions.append(added);
    m_remoteModifications.append(modified);

    // now handle removals
    contactAddModsComplete(addressbookUrl);
}

void CardDav::contactAddModsComplete(const QString &addressbookUrl)
{
    QList<KContacts::Addressee> removed;

    // fill out removed set, and remove any state data associated with removed contacts
    for (int i = 0; i < q->m_serverDeletions[addressbookUrl].size(); ++i) {
        QString guid = q->m_serverDeletions[addressbookUrl][i].guid;

        // create the contact to remove
        KContacts::Addressee doomed;
//         doomed.setUid(guid);
        if (!q->m_contactIds.contains(guid)) {
            qWarning() << "removed contact has no id";
            continue; // cannot remove it if we don't know the id
        }
        doomed.setUid(q->m_contactIds[guid]);
        removed.append(doomed);

        // update the state data
        q->m_contactUids.remove(guid);
        q->m_contactUris.remove(guid);
        q->m_contactEtags.remove(guid);
        q->m_contactIds.remove(guid);
        q->m_contactUnsupportedProperties.remove(guid);
        q->m_addressbookContactGuids[addressbookUrl].removeOne(guid);
    }

    // coalesce the removed contacts from this addressbook into the complete AMR
    m_remoteRemovals.append(removed);

    // downsync complete for this addressbook.
    // we use a singleshot to ensure that the m_deltaRequests count isn't
    // decremented synchronously to zero if the first addressbook didn't
    // have any remote additions or modifications (requiring async request).
    QTimer::singleShot(0, this, SLOT(downsyncComplete()));
}

void CardDav::downsyncComplete()
{
    // downsync complete for this addressbook
    // if this was the last outstanding addressbook, we're finished.
    m_downsyncRequests -= 1;
    if (m_downsyncRequests == 0) {
        qDebug() << "downsync complete with total AMR:"
                 << m_remoteAdditions.size() << ","
                 << m_remoteModifications.size() << ","
                 << m_remoteRemovals.size();
        emit remoteChanges(m_remoteAdditions, m_remoteModifications, m_remoteRemovals);
    }
}

void CardDav::upsyncUpdates(const QString &addressbookUrl, const QList<KContacts::Addressee> &added, const QList<KContacts::Addressee> &modified, const QList<KContacts::Addressee> &removed)
{
    qDebug() << "upsyncing updates to addressbook:" << addressbookUrl
             << ":" << added.count() << modified.count() << removed.count();

    KContacts::VCardConverter converter;

    if (added.size() == 0 && modified.size() == 0 && removed.size() == 0) {
        // nothing to upsync.  Use a singleshot to avoid synchronously
        // decrementing the m_upsyncRequests count to zero if there
        // happens to be nothing to upsync to the first addressbook.
        m_upsyncRequests += 1;
        QTimer::singleShot(0, this, SLOT(upsyncComplete()));
    } else {
        // put local additions
        for (int i = 0; i < added.size(); ++i) {
            KContacts::Addressee c = added.at(i);
            // generate a server-side uid
            QString uid = QUuid::createUuid().toString().replace(QRegularExpression(QStringLiteral("[\\-{}]")), QString());
            // transform into local-device guid
            QString guid = QStringLiteral("%1:%2").arg(q->m_accountId).arg(uid);
            // generate a valid uri
            QString uri = addressbookUrl + "/" + uid + ".vcf";
            // update our state data
            q->m_contactUids[guid] = uid;
            q->m_contactUris[guid] = uri;
//             q->m_contactIds[guid] = c.id().toString();
            // set the uid not guid so that the UID is generated.
            c.setUid(uid);
            // generate a vcard
            QString vcard =  QString(converter.createVCard(c));
            // upload
            QNetworkReply *reply = m_request->upsyncAddMod(m_serverUrl, uri, QString(), vcard);
            if (!reply) {
                emit error();
                return;
            }

            m_upsyncRequests += 1;
            reply->setProperty("addressbookUrl", addressbookUrl);
            reply->setProperty("contactGuid", guid);
            connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrorsOccurred(QList<QSslError>)));
            connect(reply, SIGNAL(finished()), this, SLOT(upsyncResponse()));
        }

        // put local modifications
        for (int i = 0; i < modified.size(); ++i) {
            KContacts::Addressee c = modified.at(i);
            // reinstate the server-side UID into the guid detail
            const QString guidstr = c.uid();
            if (guidstr.isEmpty()) {
                qWarning() << "modified contact has no guid:" << c.toString();
                continue; // TODO: this is actually an error.
            }
            QString uidstr = q->m_contactUids[guidstr];
            if (uidstr.isEmpty()) {
                qWarning() << "modified contact server uid unknown:" << c.toString() << guidstr;
                continue; // TODO: this is actually an error.
            }
            c.setUid(uidstr);
            QString vcard = QString(converter.createVCard(c));
            // upload
            QNetworkReply *reply = m_request->upsyncAddMod(m_serverUrl,
                    q->m_contactUris[guidstr],
                    q->m_contactEtags[guidstr],
                    vcard);
            if (!reply) {
                emit error();
                return;
            }

            m_upsyncRequests += 1;
            reply->setProperty("addressbookUrl", addressbookUrl);
            reply->setProperty("contactGuid", guidstr);
            connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrorsOccurred(QList<QSslError>)));
            connect(reply, SIGNAL(finished()), this, SLOT(upsyncResponse()));
        }

        // delete local removals
        for (int i = 0; i < removed.size(); ++i) {
            const QString &guidstr(removed.at(i).uid());
            QNetworkReply *reply = m_request->upsyncDeletion(m_serverUrl,
                    q->m_contactUris[guidstr],
                    q->m_contactEtags[guidstr]);
            if (!reply) {
                emit error();
                return;
            }

            // clear state data for this (deleted) contact
            q->m_contactEtags.remove(guidstr);
            q->m_contactUris.remove(guidstr);
//             q->m_contactIds.remove(guidstr);
            q->m_contactUids.remove(guidstr);
            q->m_addressbookContactGuids[addressbookUrl].removeOne(guidstr);

            m_upsyncRequests += 1;
            reply->setProperty("addressbookUrl", addressbookUrl);
            connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrorsOccurred(QList<QSslError>)));
            connect(reply, SIGNAL(finished()), this, SLOT(upsyncResponse()));
        }
    }
}

void CardDav::upsyncResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QString guid = reply->property("contactGuid").toString();
    QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        int httpError = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qWarning() << "error:" << reply->error() << "(" << httpError << ")";
        debugDumpData(QString::fromUtf8(data));
        errorOccurred(httpError);
        return;
    }

    if (!guid.isEmpty()) {
        // this is an addition or modification.
        // get the new etag value reported by the server.
        QString etag;
        Q_FOREACH(const QByteArray &header, reply->rawHeaderList()) {
            if (QString::fromUtf8(header).contains(QLatin1String("etag"), Qt::CaseInsensitive)) {
                etag = reply->rawHeader(header);
                break;
            }
        }

        if (!etag.isEmpty()) {
            qDebug() << "Got updated etag for" << guid << ":" << etag;
            q->m_contactEtags[guid] = etag;
        } else {
            // If we don't perform an additional request, the etag server-side will be different to the etag
            // we have locally, and thus on next sync we would spuriously detect a server-side modification.
            // That's ok, we'll just detect that it's spurious via data inspection during the next sync.
            qWarning() << "No updated etag provided for" << guid << ": will be reported as spurious remote modification next sync";
        }
    }

    // upsync is complete for this addressbook.
    upsyncComplete();
}

void CardDav::upsyncComplete()
{
    m_upsyncRequests -= 1;
    if (m_upsyncRequests == 0) {
        // finished upsyncing all data for all addressbooks.
        qDebug() << "upsync complete";
        emit upsyncCompleted();
    }
}
