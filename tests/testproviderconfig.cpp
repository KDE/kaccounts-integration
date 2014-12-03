/*************************************************************************************
 *  Copyright (C) 2013 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#include <QtTest/QtTest>
#include <QtCore/QObject>

#include "../src/providerconfig.h"

#include <QtXml/QDomDocument>

class testProviderConfig : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void facebookTest();
    void sipTest();
    void twitterTest();

private:
    const ProviderConfig getProviderConfig(const QString &fileName);
};

void testProviderConfig::sipTest()
{
    ProviderConfig config = getProviderConfig("sip.provider");
    QCOMPARE(config.method(), QString(""));
    QCOMPARE(config.mechanism(), QString(""));

    QMap<QString, QString> authInfo2;
    QMap<QString, QString> authInfo = config.authInfo();

    QCOMPARE(authInfo, authInfo2);
}

void testProviderConfig::facebookTest()
{
    ProviderConfig config = getProviderConfig("facebook.provider");
    QCOMPARE(config.method(), QString("oauth2"));
    QCOMPARE(config.mechanism(), QString("user_agent"));

    QMap<QString, QString> authInfo2;
    authInfo2["Host"] = "www.facebook.com";
    authInfo2["AuthPath"] = "/dialog/oauth";
    authInfo2["RedirectUri"] = "https://www.facebook.com/connect/login_success.html";
    authInfo2["Display"] = "popup";
    QMap<QString, QString> authInfo = config.authInfo();

    QCOMPARE(authInfo, authInfo2);
}

void testProviderConfig::twitterTest()
{
    ProviderConfig config = getProviderConfig("twitter.provider");
    QCOMPARE(config.method(), QString("oauth2"));
    QCOMPARE(config.mechanism(), QString("HMAC_SHA1"));

    QMap<QString, QString> authInfo2;
    authInfo2["AuthorizationEndpoint"] = "https://api.twitter.com/oauth/authorize";
    authInfo2["Callback"] = "https://wiki.ubuntu.com/";
    authInfo2["RequestEndpoint"] = "https://api.twitter.com/oauth/request_token";
    authInfo2["TokenEndpoint"] = "https://api.twitter.com/oauth/access_token";
    QMap<QString, QString> authInfo = config.authInfo();

    QCOMPARE(authInfo, authInfo2);
}

const ProviderConfig testProviderConfig::getProviderConfig(const QString& fileName)
{
    QDomDocument doc;
    QFile file(QString(TEST_DATA) + fileName);
    file.open(QIODevice::ReadOnly);
    doc.setContent(&file);

    return ProviderConfig(doc);
}

QTEST_MAIN(testProviderConfig)

#include "testproviderconfig.moc"