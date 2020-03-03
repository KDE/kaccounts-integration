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

#include "accountsmodel.h"
#include "servicesmodel.h"
#include "providersmodel.h"

#include "accountservicetogglejob.h"
#include "changeaccountdisplaynamejob.h"
#include "createaccountjob.h"
#include "removeaccountjob.h"

#include <qqml.h>

void KAccountsDeclarativePlugin::registerTypes(const char* uri)
{
    // Version 1.0
    // Consider this registration deprecated - use the one named ...Job below instead
    qmlRegisterType<CreateAccountJob>(            uri, 1, 0, "CreateAccount");

    // Version 1.1
    // Consider this registration deprecated - use the one named ...Job below instead
    qmlRegisterType<AccountServiceToggleJob>(     uri, 1, 1, "AccountServiceToggle");

    // Version 1.2
    qmlRegisterType<AccountsModel>(               uri, 1, 2, "AccountsModel");
    qmlRegisterType<ProvidersModel>(              uri, 1, 2, "ProvidersModel");
    qmlRegisterType<ServicesModel>(               uri, 1, 2, "ServicesModel");

    qmlRegisterType<AccountServiceToggleJob>(     uri, 1, 2, "AccountServiceToggleJob");
    qmlRegisterType<ChangeAccountDisplayNameJob>( uri, 1, 2, "ChangeAccountDisplayNameJob");
    qmlRegisterType<CreateAccountJob>(            uri, 1, 2, "CreateAccountJob");
    qmlRegisterType<RemoveAccountJob>(            uri, 1, 2, "RemoveAccountJob");
}
