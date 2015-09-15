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

#include "replyparser_p.h"
#include "syncer_p.h"
#include "carddav_p.h"

#include <QString>
#include <QList>
#include <QXmlStreamReader>
#include <QByteArray>
#include <QRegularExpression>
#include <QDebug>

#include <KContacts/VCardConverter>
#include <KContacts/Addressee>

namespace {
    void debugDumpData(const QString &data)
    {
        QString dbgout;
        Q_FOREACH (const QChar &c, data) {
            if (c == '\r' || c == '\n') {
                if (!dbgout.isEmpty()) {
                    qWarning() << dbgout;
                    dbgout.clear();
                }
            } else {
                dbgout += c;
            }
        }
        if (!dbgout.isEmpty()) {
            qWarning() << dbgout;
        }
    }

    QVariantMap elementToVMap(QXmlStreamReader &reader)
    {
        QVariantMap element;

        // store the attributes of the element
        QXmlStreamAttributes attrs = reader.attributes();
        while (attrs.size()) {
            QXmlStreamAttribute attr = attrs.takeFirst();
            element.insert(attr.name().toString(), attr.value().toString());
        }

        while (reader.readNext() != QXmlStreamReader::EndElement) {
            if (reader.isCharacters()) {
                // store the text of the element, if any
                QString elementText = reader.text().toString();
                if (!elementText.isEmpty()) {
                    element.insert(QLatin1String("@text"), elementText);
                }
            } else if (reader.isStartElement()) {
                // recurse if necessary.
                QString subElementName = reader.name().toString();
                QVariantMap subElement = elementToVMap(reader);
                if (element.contains(subElementName)) {
                    // already have an element with this name.
                    // create a variantlist and append.
                    QVariant existing = element.value(subElementName);
                    QVariantList subElementList;
                    if (existing.type() == QVariant::Map) {
                        // we need to convert the value into a QVariantList
                        subElementList << existing.toMap();
                    } else if (existing.type() == QVariant::List) {
                        subElementList = existing.toList();
                    }
                    subElementList << subElement;
                    element.insert(subElementName, subElementList);
                } else {
                    // first element with this name.  insert as a map.
                    element.insert(subElementName, subElement);
                }
            }
        }

        return element;
    }

    QVariantMap xmlToVMap(QXmlStreamReader &reader)
    {
        QVariantMap retn;
        while (!reader.atEnd() && !reader.hasError() && reader.readNextStartElement()) {
            QString elementName = reader.name().toString();
            QVariantMap element = elementToVMap(reader);
            retn.insert(elementName, element);
        }
        return retn;
    }
}

ReplyParser::ReplyParser(Syncer *parent)
    : q(parent)
{
}

ReplyParser::~ReplyParser()
{
}

QString ReplyParser::parseUserPrincipal(const QByteArray &userInformationResponse, ReplyParser::ResponseType *responseType) const
{
    /* We expect a response of the form:
        HTTP/1.1 207 Multi-status
        Content-Type: application/xml; charset=utf-8

        <d:multistatus xmlns:d="DAV:">
            <d:response>
                <d:href>/</d:href>
                <d:propstat>
                    <d:prop>
                        <d:current-user-principal>
                            <d:href>/principals/users/johndoe/</d:href>
                        </d:current-user-principal>
                    </d:prop>
                    <d:status>HTTP/1.1 200 OK</d:status>
                </d:propstat>
            </d:response>
        </d:multistatus>

      Note however that some CardDAV servers return addressbook
      information instead of user principal information.
    */
    debugDumpData(QString::fromUtf8(userInformationResponse));
    QXmlStreamReader reader(userInformationResponse);
    QVariantMap vmap = xmlToVMap(reader);
    QVariantMap multistatusMap = vmap[QLatin1String("multistatus")].toMap();
    if (multistatusMap[QLatin1String("response")].type() == QVariant::List) {
        // This should not be the case for a UserPrincipal response.
        *responseType = ReplyParser::AddressbookInformationResponse;
        return QString();
    }

    // Only one response - this could be either a UserPrincipal response
    // or an AddressbookInformation response.
    QVariantMap response = multistatusMap[QLatin1String("response")].toMap();
    QString statusText = response.value("propstat").toMap().value("status").toMap().value("@text").toString();
    QString userPrincipal = response.value("propstat").toMap().value("prop").toMap()
            .value("current-user-principal").toMap().value("href").toMap().value("@text").toString();
    QString ctag = response.value("propstat").toMap().value("prop").toMap().value("getctag").toMap().value("@text").toString();

    if (!statusText.contains(QLatin1String("200 OK"))) {
        qWarning() << "invalid status response to current user information request:" << statusText;
    } else if (userPrincipal.isEmpty() && !ctag.isEmpty()) {
        // this server has responded with an addressbook information response.
        qWarning() << "addressbook information response to current user information request:" << statusText;
        *responseType = ReplyParser::AddressbookInformationResponse;
        return QString();
    }

    *responseType = ReplyParser::UserPrincipalResponse;
    return userPrincipal;
}

QString ReplyParser::parseAddressbookHome(const QByteArray &addressbookUrlsResponse) const
{
    /* We expect a response of the form:
        HTTP/1.1 207 Multi-status
        Content-Type: application/xml; charset=utf-8

        <d:multistatus xmlns:d="DAV:" xmlns:card="urn:ietf:params:xml:ns:carddav">
            <d:response>
                <d:href>/</d:href>
                <d:propstat>
                    <d:prop>
                        <c:addressbook-home-set>
                            <d:href>/addressbooks/johndoe/</d:href>
                        </c:addressbook-home-set>
                    </d:prop>
                    <d:status>HTTP/1.1 200 OK</d:status>
                </d:propstat>
            </d:response>
        </d:multistatus>
    */
    debugDumpData(QString::fromUtf8(addressbookUrlsResponse));
    QXmlStreamReader reader(addressbookUrlsResponse);
    QString statusText;
    QString addressbookHome;

    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name().toString() == QLatin1String("addressbook-home-set")) {
                if (reader.readNextStartElement() && reader.name().toString() == QLatin1String("href")) {
                    addressbookHome = reader.readElementText();
                }
            } else if (reader.name().toString() == QLatin1String("status")) {
                statusText = reader.readElementText();
            }
        }
    }

    if (!statusText.contains(QLatin1String("200 OK"))) {
        qWarning() << "invalid status response to addressbook home request:" << statusText;
    }

    return addressbookHome;
}

QList<ReplyParser::AddressBookInformation> ReplyParser::parseAddressbookInformation(const QByteArray &addressbookInformationResponse) const
{
    /* We expect a response of the form:
        <d:multistatus xmlns:d="DAV:" xmlns:cs="http://calendarserver.org/ns/">
            <d:response>
                <d:href>/addressbooks/johndoe/contacts/</d:href>
                <d:propstat>
                    <d:prop>
                        <d:displayname>My Address Book</d:displayname>
                        <cs:getctag>3145</cs:getctag>
                        <d:sync-token>http://sabredav.org/ns/sync-token/3145</d:sync-token>
                    </d:prop>
                    <d:status>HTTP/1.1 200 OK</d:status>
                </d:propstat>
            </d:response>
        </d:multistatus>
    */
    debugDumpData(QString::fromUtf8(addressbookInformationResponse));
    QXmlStreamReader reader(addressbookInformationResponse);
    QList<ReplyParser::AddressBookInformation> infos;

    QVariantMap vmap = xmlToVMap(reader);
    QVariantMap multistatusMap = vmap[QLatin1String("multistatus")].toMap();
    QVariantList responses;
    if (multistatusMap[QLatin1String("response")].type() == QVariant::List) {
        // multiple addressbooks.
        responses = multistatusMap[QLatin1String("response")].toList();
    } else {
        // only one addressbook.
        QVariantMap response = multistatusMap[QLatin1String("response")].toMap();
        responses << response;
    }

    Q_FOREACH (const QVariant &rv, responses) {
        QVariantMap rmap = rv.toMap();
        ReplyParser::AddressBookInformation currInfo;
        currInfo.url = QUrl::fromPercentEncoding(rmap.value("href").toMap().value("@text").toString().toUtf8());
        currInfo.ctag = rmap.value("propstat").toMap().value("prop").toMap().value("getctag").toMap().value("@text").toString();
        currInfo.syncToken = rmap.value("propstat").toMap().value("prop").toMap().value("sync-token").toMap().value("@text").toString();
        currInfo.displayName = rmap.value("propstat").toMap().value("prop").toMap().value("displayname").toMap().value("@text").toString();
        QStringList resourceTypeKeys = rmap.value("propstat").toMap().value("prop").toMap().value("resourcetype").toMap().keys();
        if (resourceTypeKeys.isEmpty()
                || (resourceTypeKeys.size() == 1 && resourceTypeKeys.contains(QStringLiteral("collection"), Qt::CaseInsensitive))
                || (resourceTypeKeys.contains(QStringLiteral("addressbook"), Qt::CaseInsensitive))) {
            // This is probably a carddav addressbook collection.
            // Despite section 5.2 of RFC6352 stating that a CardDAV
            // server MUST return the 'addressbook' value in the resource types
            // property, some CardDAV implementations (eg, Memotoo) do not.
            qWarning() << "parsing information for addressbook:" << currInfo.url;
        } else {
            // the resource is explicitly described as non-addressbook resource.
            qWarning() << "ignoring non-addressbook response";
            continue;
        }
        QString status = rmap.value("propstat").toMap().value("status").toMap().value("@text").toString();
        if (status.contains(QRegularExpression("2[0-9][0-9]"))) { // any HTTP 2xx response
            if (currInfo.ctag.isEmpty() && currInfo.syncToken.isEmpty()) {
                qWarning() << "ignoring addressbook:" << currInfo.url << "due to lack of ctag";
            } else {
                qWarning() << "found valid addressbook:" << currInfo.url;
                infos.append(currInfo);
            }
        } else {
            qWarning() << "ignoring addressbook:" << currInfo.url << "due to invalid status:" << status;
        }
    }

    return infos;
}

QList<ReplyParser::ContactInformation> ReplyParser::parseSyncTokenDelta(const QByteArray &syncTokenDeltaResponse, QString *newSyncToken) const
{
    /* We expect a response of the form:
        <?xml version="1.0" encoding="utf-8" ?>
        <d:multistatus xmlns:d="DAV:">
            <d:response>
                <d:href>/addressbooks/johndoe/contacts/newcard.vcf</d:href>
                <d:propstat>
                    <d:prop>
                        <d:getetag>"33441-34321"</d:getetag>
                    </d:prop>
                    <d:status>HTTP/1.1 200 OK</d:status>
                </d:propstat>
            </d:response>
            <d:response>
                <d:href>/addressbooks/johndoe/contacts/updatedcard.vcf</d:href>
                <d:propstat>
                    <d:prop>
                        <d:getetag>"33541-34696"</d:getetag>
                    </d:prop>
                    <d:status>HTTP/1.1 200 OK</d:status>
                </d:propstat>
            </d:response>
            <d:response>
                <d:href>/addressbooks/johndoe/contacts/deletedcard.vcf</d:href>
                <d:status>HTTP/1.1 404 Not Found</d:status>
            </d:response>
            <d:sync-token>http://sabredav.org/ns/sync/5001</d:sync-token>
         </d:multistatus>
    */
    debugDumpData(QString::fromUtf8(syncTokenDeltaResponse));
    QList<ReplyParser::ContactInformation> info;
    QXmlStreamReader reader(syncTokenDeltaResponse);
    QVariantMap vmap = xmlToVMap(reader);
    QVariantMap multistatusMap = vmap[QLatin1String("multistatus")].toMap();
    if (newSyncToken) {
        *newSyncToken = multistatusMap.value("sync-token").toMap().value("@text").toString();
    }

    QVariantList responses;
    if (multistatusMap[QLatin1String("response")].type() == QVariant::List) {
        // multiple updates in the delta.
        responses = multistatusMap[QLatin1String("response")].toList();
    } else {
        // only one update in the delta.
        QVariantMap response = multistatusMap[QLatin1String("response")].toMap();
        responses << response;
    }

    Q_FOREACH (const QVariant &rv, responses) {
        QVariantMap rmap = rv.toMap();
        ReplyParser::ContactInformation currInfo;
        currInfo.uri = QUrl::fromPercentEncoding(rmap.value("href").toMap().value("@text").toString().toUtf8());
        currInfo.etag = rmap.value("propstat").toMap().value("prop").toMap().value("getetag").toMap().value("@text").toString();
        QMap<QString, QString>::const_iterator it = q->m_contactUris.constBegin();
        for ( ; it != q->m_contactUris.constEnd(); ++it) {
            if (it.value() == currInfo.uri) {
                currInfo.guid = it.key();
            }
        }
        QString status = rmap.value("propstat").toMap().value("status").toMap().value("@text").toString();
        if (status.contains(QLatin1String("200 OK"))) {
            if (!currInfo.uri.endsWith(QStringLiteral(".vcf"), Qt::CaseInsensitive)) {
                // this is probably a response for the addressbook resource,
                // rather than for a contact resource within the addressbook.
                qWarning() << "ignoring non-contact resource:" << currInfo.uri << currInfo.etag << status;
                continue;
            }
            currInfo.modType = currInfo.guid.isEmpty()
                             ? ReplyParser::ContactInformation::Addition
                             : ReplyParser::ContactInformation::Modification;
        } else if (status.contains(QLatin1String("404 Not Found"))) {
            currInfo.modType = ReplyParser::ContactInformation::Deletion;
        } else {
            qWarning() << "unknown response:" << currInfo.uri << currInfo.etag << status;
        }
        info.append(currInfo);
    }

    return info;
}

QList<ReplyParser::ContactInformation> ReplyParser::parseContactMetadata(const QByteArray &contactMetadataResponse, const QString &addressbookUrl) const
{
    /* We expect a response of the form:
        HTTP/1.1 207 Multi-status
        Content-Type: application/xml; charset=utf-8

        <d:multistatus xmlns:d="DAV:" xmlns:card="urn:ietf:params:xml:ns:carddav">
            <d:response>
                <d:href>/addressbooks/johndoe/contacts/abc-def-fez-123454657.vcf</d:href>
                <d:propstat>
                    <d:prop>
                        <d:getetag>"2134-888"</d:getetag>
                    </d:prop>
                    <d:status>HTTP/1.1 200 OK</d:status>
                </d:propstat>
            </d:response>
            <d:response>
                <d:href>/addressbooks/johndoe/contacts/acme-12345.vcf</d:href>
                <d:propstat>
                    <d:prop>
                        <d:getetag>"9999-2344""</d:getetag>
                    </d:prop>
                    <d:status>HTTP/1.1 200 OK</d:status>
                </d:propstat>
            </d:response>
        </d:multistatus>
    */
    debugDumpData(QString::fromUtf8(contactMetadataResponse));
    QList<ReplyParser::ContactInformation> info;
    QXmlStreamReader reader(contactMetadataResponse);
    QVariantMap vmap = xmlToVMap(reader);
    QVariantMap multistatusMap = vmap[QLatin1String("multistatus")].toMap();
    QVariantList responses;
    if (multistatusMap[QLatin1String("response")].type() == QVariant::List) {
        // multiple updates in the delta.
        responses = multistatusMap[QLatin1String("response")].toList();
    } else {
        // only one update in the delta.
        QVariantMap response = multistatusMap[QLatin1String("response")].toMap();
        responses << response;
    }

    QSet<QString> seenUris;
    Q_FOREACH (const QVariant &rv, responses) {
        QVariantMap rmap = rv.toMap();
        ReplyParser::ContactInformation currInfo;
        currInfo.uri = QUrl::fromPercentEncoding(rmap.value("href").toMap().value("@text").toString().toUtf8());
        currInfo.etag = rmap.value("propstat").toMap().value("prop").toMap().value("getetag").toMap().value("@text").toString();
        QString status = rmap.value("propstat").toMap().value("status").toMap().value("@text").toString();
        if (!currInfo.uri.endsWith(QStringLiteral(".vcf"), Qt::CaseInsensitive)) {
            // this is probably a response for the addressbook resource,
            // rather than for a contact resource within the addressbook.
            qWarning() << "ignoring non-contact resource:" << currInfo.uri << currInfo.etag << status;
            continue;
        }
        QMap<QString, QString>::const_iterator it = q->m_contactUris.constBegin();
        for ( ; it != q->m_contactUris.constEnd(); ++it) {
            if (it.value() == currInfo.uri) {
                currInfo.guid = it.key();
            }
        }
        if (status.contains(QLatin1String("200 OK"))) {
            seenUris.insert(currInfo.uri);
            currInfo.modType = currInfo.guid.isEmpty()
                             ? ReplyParser::ContactInformation::Addition
                             : ReplyParser::ContactInformation::Modification;
            // only append if it's an addition or an actual modification
            // the etag will have changed since the last time we saw it,
            // if the contact has been modified server-side since last sync.
            if (currInfo.modType == ReplyParser::ContactInformation::Addition) {
                qDebug() << "Resource" << currInfo.uri << "was added on server with etag" << currInfo.etag;
                info.append(currInfo);
            } else if (q->m_contactEtags[currInfo.guid] != currInfo.etag) {
                qDebug() << "Resource" << currInfo.uri << "with guid" << currInfo.guid << "was modified on server.";
                qDebug() << "Old etag:" << q->m_contactEtags[currInfo.guid] << "New etag:" << currInfo.etag;
                info.append(currInfo);
            } else {
                qDebug() << "Resource" << currInfo.uri << "with guid" << currInfo.guid << "is unchanged since last sync with etag" << currInfo.etag;
            }
        } else {
            qWarning() << "unknown response:" << currInfo.uri << currInfo.etag << status;
        }
    }

    // we now need to determine deletions.
    QStringList contactGuidsInAddressbook = q->m_addressbookContactGuids[addressbookUrl];
    Q_FOREACH (const QString &guid, contactGuidsInAddressbook) {
        const QString &uri(q->m_contactUris[guid]);
        if (!seenUris.contains(uri)) {
            // this uri wasn't listed in the report, so this contact must have been deleted.
            qDebug() << "Resource" << uri << "with guid" << guid << "was deleted on server";
            ReplyParser::ContactInformation currInfo;
            currInfo.etag = q->m_contactEtags[guid];
            currInfo.uri = uri;
            currInfo.guid = guid;
            currInfo.modType = ReplyParser::ContactInformation::Deletion;
            info.append(currInfo);
        }
    }

    return info;
}

QMap<QString, ReplyParser::FullContactInformation> ReplyParser::parseContactData(const QByteArray &contactData) const
{
    /* We expect a response of the form:
        HTTP/1.1 207 Multi-status
        Content-Type: application/xml; charset=utf-8

        <d:multistatus xmlns:d="DAV:" xmlns:card="urn:ietf:params:xml:ns:carddav">
            <d:response>
                <d:href>/addressbooks/johndoe/contacts/abc-def-fez-123454657.vcf</d:href>
                <d:propstat>
                    <d:prop>
                        <d:getetag>"2134-314"</d:getetag>
                        <card:address-data>BEGIN:VCARD
                            VERSION:3.0
                            FN:My Mother
                            UID:abc-def-fez-1234546578
                            END:VCARD
                        </card:address-data>
                    </d:prop>
                    <d:status>HTTP/1.1 200 OK</d:status>
                </d:propstat>
            </d:response>
            <d:response>
                <d:href>/addressbooks/johndoe/contacts/someapplication-12345678.vcf</d:href>
                <d:propstat>
                    <d:prop>
                        <d:getetag>"5467-323"</d:getetag>
                        <card:address-data>BEGIN:VCARD
                            VERSION:3.0
                            FN:Your Mother
                            UID:foo-bar-zim-gir-1234567
                            END:VCARD
                        </card:address-data>
                    </d:prop>
                    <d:status>HTTP/1.1 200 OK</d:status>
                </d:propstat>
            </d:response>
        </d:multistatus>
    */
    debugDumpData(QString::fromUtf8(contactData));
    QXmlStreamReader reader(contactData);
    QVariantMap vmap = xmlToVMap(reader);
    QVariantMap multistatusMap = vmap[QLatin1String("multistatus")].toMap();
    QVariantList responses;
    if (multistatusMap[QLatin1String("response")].type() == QVariant::List) {
        // multiple updates in the delta.
        responses = multistatusMap[QLatin1String("response")].toList();
    } else {
        // only one update in the delta.
        QVariantMap response = multistatusMap[QLatin1String("response")].toMap();
        responses << response;
    }

    QMap<QString, ReplyParser::FullContactInformation> uriToContactData;
    KContacts::VCardConverter converter;
    Q_FOREACH (const QVariant &rv, responses) {
        QVariantMap rmap = rv.toMap();
        QString uri = QUrl::fromPercentEncoding(rmap.value("href").toMap().value("@text").toString().toUtf8());
        QString etag = rmap.value("propstat").toMap().value("prop").toMap().value("getetag").toMap().value("@text").toString();
        QString vcard = rmap.value("propstat").toMap().value("prop").toMap().value("address-data").toMap().value("@text").toString();

        // import the data as a vCard
//         bool ok = true;

        KContacts::Addressee result = converter.parseVCard(vcard.toUtf8());
//         if (!ok) {
//             continue;
//         }

        // fix up various details of the contact.
//         QContactGuid guid = importedContact.detail<QContactGuid>();
        const QString uid = result.uid();
        if (uid.isEmpty()) {
            qWarning() << "contact import from vcard has no UID:\n" << vcard;
            continue;
        }
        bool found = false;
        QMap<QString,QString>::const_iterator it = q->m_contactUids.constBegin();
        for ( ; it != q->m_contactUids.constEnd(); ++it) {
            // see if the UID exists in our map already
            if (it.value() == uid) {
                // it does; use the local-device GUID instead.
                result.setUid(it.key());
                found = true;
                break;
            }
        }
        if (!found) {
            // this is a server addition.  mutate the uid into a per-account device guid.
            result.setUid(QStringLiteral("%1:%2").arg(q->m_accountId).arg(uid));
            // also set the guid to uid mapping for the server-side addition.
            q->m_contactUids.insert(result.uid(), uid);
        }

//         importedContact.saveDetail(&guid);

        // and insert into the return map.
        ReplyParser::FullContactInformation fci;
        fci.contact = result; // importedContact;
//         fci.unsupportedProperties = result.second;
        fci.etag = etag;
        uriToContactData.insert(uri, fci);
    }

    return uriToContactData;
}

