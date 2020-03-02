/*************************************************************************************
 *  Copyright (C) 2015 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "kaccountsdeclarativeplugin.h"
#include "createaccountjob.h"
#include "accountservicetogglejob.h"
#include "removeaccountjob.h"
#include "accountsmodel.h"
#include <providersmodel.h>
#include "servicesmodel.h"
#include <qqml.h>

void KAccountsDeclarativePlugin::registerTypes(const char* uri)
{
    qmlRegisterType<CreateAccountJob>(uri, 1, 0, "CreateAccount");
    qmlRegisterType<AccountServiceToggleJob>(uri, 1, 1, "AccountServiceToggle");
    qmlRegisterType<RemoveAccountJob>(uri, 1, 2, "RemoveAccount");
    qmlRegisterType<ProvidersModel>(uri, 1, 2, "ProvidersModel");
    qmlRegisterType<AccountsModel>(uri, 1, 2, "AccountsModel");
    qmlRegisterType<ServicesModel>(uri, 1, 2, "ServicesModel");
    qmlRegisterType<CreateAccountJob>(uri, 1, 0, "CreateAccountJob");
    qmlRegisterType<AccountServiceToggleJob>(uri, 1, 1, "AccountServiceToggleJob");
}
