/*
 *  SPDX-FileCopyrightText: 2015 Aleix Pol <aleixpol@kde.org>
 *  SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kaccountsdeclarativeplugin.h"

#include "accountsmodel.h"
#include "providersmodel.h"
#include "servicesmodel.h"

#include "accountservicetogglejob.h"
#include "changeaccountdisplaynamejob.h"
#include "createaccountjob.h"
#include "removeaccountjob.h"

#include <qqml.h>

void KAccountsDeclarativePlugin::registerTypes(const char *uri)
{
    // Version 1.0
    // Consider this registration deprecated - use the one named ...Job below instead
    qmlRegisterType<KAccounts::CreateAccountJob>(uri, 1, 0, "CreateAccount");

    // Version 1.1
    // Consider this registration deprecated - use the one named ...Job below instead
    qmlRegisterType<KAccounts::AccountServiceToggleJob>(uri, 1, 1, "AccountServiceToggle");

    // Version 1.2
    qmlRegisterType<KAccounts::AccountsModel>(uri, 1, 2, "AccountsModel");
    qmlRegisterType<KAccounts::ProvidersModel>(uri, 1, 2, "ProvidersModel");
    qmlRegisterType<KAccounts::ServicesModel>(uri, 1, 2, "ServicesModel");

    qmlRegisterType<KAccounts::AccountServiceToggleJob>(uri, 1, 2, "AccountServiceToggleJob");
    qmlRegisterType<KAccounts::ChangeAccountDisplayNameJob>(uri, 1, 2, "ChangeAccountDisplayNameJob");
    qmlRegisterType<KAccounts::CreateAccountJob>(uri, 1, 2, "CreateAccountJob");
    qmlRegisterType<KAccounts::RemoveAccountJob>(uri, 1, 2, "RemoveAccountJob");
}
