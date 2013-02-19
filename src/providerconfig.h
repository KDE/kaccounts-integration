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

#ifndef PROVIDER_CONFIG_H
#define PROVIDER_CONFIG_H

#include <QtCore/QMap>
#include <QtCore/QString>

class QDomNodeList;
class QDomDocument;
class ProviderConfig
{
    public:
        ProviderConfig(const QDomDocument &dom);
        ~ProviderConfig();

        const QString method() const;
        const QString mechanism() const;
        const QMap<QString, QString> authInfo() const;
    private:
        bool authMethodHasMoreInfo() const;
        void parseAuthMethod(const QDomNodeList &groupList);
        void parseAuthInfo(const QDomNodeList &groupList);

        QString m_method;
        QString m_mechanism;
        QMap<QString, QString> m_authInfo;
};
#endif //PROVIDER_CONFIG_H