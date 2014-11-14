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

#include "providerconfig.h"

#include <QDebug>
#include <QDomDocument>

ProviderConfig::ProviderConfig(const QDomDocument &dom)
{
    QDomNodeList groupList = dom.elementsByTagName("group");

    parseAuthMethod(groupList);

    if (!authMethodHasMoreInfo()) {
        return;
    }

    parseAuthInfo(groupList);
}

ProviderConfig::~ProviderConfig()
{

}

const QString ProviderConfig::method() const
{
    return m_method;
}

const QString ProviderConfig::mechanism() const
{
    return m_mechanism;
}

const QMap< QString, QString > ProviderConfig::authInfo() const
{
    return m_authInfo;
}

bool ProviderConfig::authMethodHasMoreInfo() const
{
    if (m_method == "oauth2") {
        return true;
    }

    return false;
}

void ProviderConfig::parseAuthMethod(const QDomNodeList &groupList)
{
    QDomElement setting;
    QDomElement auth = groupList.at(0).toElement();
    QDomNodeList settings = auth.elementsByTagName("setting");

    for (int i = 0; i < settings.size(); ++i) {
        setting = settings.at(i).toElement();

        if (setting.attribute("name") == "method") {
            m_method = setting.text();
        }
        if (setting.attribute("name") == "mechanism") {
            m_mechanism = setting.text();
        }
    }
}

void ProviderConfig::parseAuthInfo(const QDomNodeList &groupList)
{
    QDomElement setting;
    QDomNodeList settings;
    settings = groupList.at(2).toElement().elementsByTagName("setting");
    for (int i = 0; i < settings.size(); ++i) {
        setting = settings.at(i).toElement();
        if (!setting.hasAttribute("name")){
            continue;
        }

        m_authInfo.insert(setting.attribute("name"), setting.text());
    }
}
