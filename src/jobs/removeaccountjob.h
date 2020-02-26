/*************************************************************************************
 *  Copyright (C) 2020 by Dan Leinir Turthra Jensen <admin@leinir.dk>                *
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

#ifndef REMOVEACCOUNT_H
#define REMOVEACCOUNT_H

#include "kaccounts_export.h"

#include <kjob.h>

#include <QString>

class KACCOUNTS_EXPORT RemoveAccountJob : public KJob
{
    Q_OBJECT
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
public:
    explicit RemoveAccountJob(QObject* parent = nullptr);
    virtual ~RemoveAccountJob();

    void start() override;

    QString accountId() const;
    void setAccountId(const QString& accountId);
    Q_SIGNAL void accountIdChanged();
private:
    class Private;
    Private* d;
};
#endif//REMOVEACCOUNT_H
