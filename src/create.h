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

#ifndef CREATE_H
#define CREATE_H

#include <QtCore/QObject>

class QDialog;
class QWidget;
class Ui_createForm;

namespace Accounts
{
    class Manager;
}

class Create : public QObject
{
Q_OBJECT

    public:
        Create (QWidget *parent);
        virtual ~Create();

        QWidget* widget();

    private Q_SLOTS:
        void fillInterface();
        void createAccount();

    Q_SIGNALS:
        void newAccount(const QString &type, const QString &name);

    private:
        QDialog *m_dialog;
        QWidget *m_parent;
        Ui_createForm *m_form;
        Accounts::Manager *m_manager;

};
#endif// CREATE_H
