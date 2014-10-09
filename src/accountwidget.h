/*************************************************************************************
 *  Copyright (C) 2012-2013 by Alejandro Fiestas Olivares <afiestas@kde.org>         *
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

#ifndef ACCOUNTWIDGET_H
#define ACCOUNTWIDGET_H

#include "ui_services.h"
#include "create.h"

#include <QPointer>

namespace Accounts {
    class Account;
};

class QCheckBox;

class AccountWidget : public QWidget, Ui::Services
{
    Q_OBJECT

public:
    explicit AccountWidget(Accounts::Account *model, QWidget *parent);
    virtual ~AccountWidget();

    void setAccount(Accounts::Account *account);
public Q_SLOTS:
    void serviceEnabledChanged(const QString &serviceName, bool enabled);
    void serviceChanged(bool enabled);

private:
    QHash<QString, QCheckBox*> m_checkboxes;
    QPointer<Accounts::Account> m_account;
    Accounts::Manager *m_manager;
};

#endif //ACCOUNTWIDGET_H
