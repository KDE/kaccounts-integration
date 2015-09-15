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

#ifndef CARDDAV_P_H
#define CARDDAV_P_H

#include "requestgenerator_p.h"
#include "replyparser_p.h"

#include <QObject>
#include <QMultiMap>
#include <QList>
#include <QMap>
#include <QString>
#include <QSet>
#include <QSslError>

// #include <QContact>
// #include <QVersitContactImporterPropertyHandlerV2>
// #include <QVersitContactExporterDetailHandlerV2>

// QTCONTACTS_USE_NAMESPACE
// QTVERSIT_USE_NAMESPACE

class Syncer;
class CardDav : public QObject
{
    Q_OBJECT

public:
    CardDav(Syncer *parent,
            const QString &serverUrl,
            const QString &addressbookPath,
            const QString &username,
            const QString &password);
    CardDav(Syncer *parent,
            const QString &serverUrl,
            const QString &addressbookPath,
            const QString &accessToken);
    ~CardDav();

    void determineRemoteAMR();
    void upsyncUpdates(const QString &addressbookUrl,
                       const QList<KContacts::Addressee> &added,
                       const QList<KContacts::Addressee> &modified,
                       const QList<KContacts::Addressee> &removed);

Q_SIGNALS:
    void error(int errorCode = 0);
    void remoteChanges(const QList<KContacts::Addressee> &added,
                       const QList<KContacts::Addressee> &modified,
                       const QList<KContacts::Addressee> &removed);
    void upsyncCompleted();

private:
    void fetchUserInformation();
    void fetchAddressbookUrls(const QString &userPath);
    void fetchAddressbooksInformation(const QString &addressbooksHomePath);
    void downsyncAddressbookContent(const QList<ReplyParser::AddressBookInformation> &infos);
    void fetchImmediateDelta(const QString &addressbookUrl, const QString &syncToken);
    void fetchContactMetadata(const QString &addressbookUrl);
    void fetchContacts(const QString &addressbookUrl, const QList<ReplyParser::ContactInformation> &amrInfo);

private Q_SLOTS:
    void sslErrorsOccurred(const QList<QSslError> &errors);
    void userInformationResponse();
    void addressbookUrlsResponse();
    void addressbooksInformationResponse();
    void immediateDeltaResponse();
    void contactMetadataResponse();
    void contactsResponse();
    void downsyncComplete();
    void upsyncResponse();
    void upsyncComplete();
    void errorOccurred(int httpError);

private:
    void contactAddModsComplete(const QString &addressbookUrl);

    enum DiscoveryStage {
        DiscoveryStarted = 0,
        DiscoveryRedirected,
        DiscoveryTryRoot
    };

    Syncer *q;
    RequestGenerator *m_request;
    ReplyParser *m_parser;
    QString m_serverUrl;
    QString m_addressbookPath;
    DiscoveryStage m_discoveryStage;

    QList<KContacts::Addressee> m_remoteAdditions;
    QList<KContacts::Addressee> m_remoteModifications;
    QList<KContacts::Addressee> m_remoteRemovals;
    int m_downsyncRequests;
    int m_upsyncRequests;
};

// class CardDavVCardConverter : public QVersitContactImporterPropertyHandlerV2,
//                               public QVersitContactExporterDetailHandlerV2
// {
// public:
//     CardDavVCardConverter();
//     ~CardDavVCardConverter();
//
//     // QVersitContactImporterPropertyHandlerV2
//     void propertyProcessed(const QVersitDocument &d, const QVersitProperty &property,
//                            const QContact &c, bool *alreadyProcessed,
//                            QList<QContactDetail> *updatedDetails);
//     void documentProcessed(const QVersitDocument &d, QContact *c);
//
//     // QVersitContactExporterDetailHandlerV2
//     void contactProcessed(const QContact &c, QVersitDocument *d);
//     void detailProcessed(const QContact &c, const QContactDetail &detail,
//                          const QVersitDocument &d, QSet<int> *processedFields,
//                          QList<QVersitProperty> *toBeRemoved, QList<QVersitProperty> *toBeAdded);
//
//     // API exposed to clients
//     QPair<QContact, QStringList> convertVCardToContact(const QString &vcard, bool *ok);
//     QString convertContactToVCard(const QContact &c, const QStringList &unsupportedProperties);
//
// private:
//     static QStringList supportedPropertyNames();
//     QString convertPropertyToString(const QVersitProperty &p) const;
//     QMap<QString, QStringList> m_unsupportedProperties; // uid -> unsupported properties
//     QStringList m_tempUnsupportedProperties;
// };

#endif // CARDDAV_P_H

