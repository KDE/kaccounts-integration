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

#ifndef REQUESTGENERATOR_P_H
#define REQUESTGENERATOR_P_H

#include <QList>
#include <QString>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class Syncer;
class RequestGenerator
{
public:
    RequestGenerator(Syncer *parent, const QString &username, const QString &password);
    RequestGenerator(Syncer *parent, const QString &accessToken);

    QNetworkReply *currentUserInformation(const QString &serverUrl);
    QNetworkReply *addressbookUrls(const QString &serverUrl, const QString &userPath);
    QNetworkReply *addressbooksInformation(const QString &serverUrl, const QString &userAddressbooksPath);
    QNetworkReply *addressbookInformation(const QString &serverUrl, const QString &addressbookPath);
    QNetworkReply *syncTokenDelta(const QString &serverUrl, const QString &addressbookUrl, const QString &syncToken);
    QNetworkReply *contactEtags(const QString &serverUrl, const QString &addressbookPath);
    QNetworkReply *contactData(const QString &serverUrl, const QString &addressbookPath, const QStringList &contactEtags);
    QNetworkReply *contactMultiget(const QString &serverUrl, const QString &addressbookPath, const QStringList &contactUris);
    QNetworkReply *upsyncAddMod(const QString &serverUrl, const QString &contactPath, const QString &etag, const QString &vcard);
    QNetworkReply *upsyncDeletion(const QString &serverUrl, const QString &contactPath, const QString &etag);

private:
    QNetworkReply *generateRequest(const QString &url,
                                   const QString &path,
                                   const QString &depth,
                                   const QString &requestType,
                                   const QString &request) const;
    QNetworkReply *generateUpsyncRequest(const QString &url,
                                         const QString &path,
                                         const QString &ifMatch,
                                         const QString &contentType,
                                         const QString &requestType,
                                         const QString &request) const;
    Syncer *q;
    QString m_username;
    QString m_password;
    QString m_accessToken;
};

#endif // REQUESTGENERATOR_P_H
