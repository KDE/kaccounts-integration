/*
 * Copyright 2014  Martin Klapetek <mklapetek@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef KACCOUNTSDPLUGIN_H
#define KACCOUNTSDPLUGIN_H

#include "kaccounts_export.h"

#include <QObject>

class KACCOUNTS_EXPORT KAccountsDPlugin : public QObject
{
    Q_OBJECT

public:
    KAccountsDPlugin(QObject *parent = 0);
    virtual ~KAccountsDPlugin();

public Q_SLOTS:
    virtual void onAccountAdded(quint32 id) = 0;
    virtual void onAccountRemoved(quint32 id) = 0;
};

Q_DECLARE_INTERFACE(KAccountsDPlugin, "org.kde.kaccounts.DPlugin")

#endif // KACCOUNTSDPLUGIN_H
