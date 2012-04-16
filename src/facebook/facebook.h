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

#ifndef FACEBOOK_H
#define FACEBOOK_H

#include <QWizard>
#include <QHash>

class FacebookWizard : public QWizard
{
    Q_OBJECT
    public:
        explicit FacebookWizard(QWidget* parent);
        virtual ~FacebookWizard();

        virtual void done(int result);

        void setUsername(const QString &username);
        void setPassword(const QString &password);
        void setAccessToken(const QString &accessToken);
        void setFacebookUsername(const QString &username);

        const QString username() const;
        const QString password() const;
        const QString accessToken() const;
        const QString facebookUsername() const;

        void activateOption(const QString& name, bool checked);

    Q_SIGNALS:
        void newAccount(const QString &type, const QString &name);

    private:
        QString m_username;
        QString m_password;
        QString m_accessToken;
        QString m_facebookUsername;

        QHash<QString, int> m_services;
};

#endif //FACEBOOK_H