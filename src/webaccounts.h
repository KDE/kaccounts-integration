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

#ifndef webaccounts_H
#define webaccounts_H

#include <kcmodule.h>

class Create;
class QModelIndex;
class AccountsModel;
class AccountWidget;
class QStackedLayout;
class QItemSelectionModel;

namespace Ui {
    class KCMWebAccounts;
}

class WebAccounts : public KCModule
{
    Q_OBJECT
public:
    WebAccounts(QWidget *parent, const QVariantList &);
    virtual ~WebAccounts();

private Q_SLOTS:
    void addBtnClicked();
    void rmBtnClicked();
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    Create *m_create;
    QStackedLayout *m_layout;
    Ui::KCMWebAccounts *m_ui;
    AccountsModel *m_model;
    AccountWidget *m_accWidget;
    QItemSelectionModel *m_selectionModel;
};

#endif // webaccounts_H
