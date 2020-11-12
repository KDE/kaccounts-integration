/*
 * Copyright 2014 (C) Martin Klapetek <mklapetek@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef KACCOUNTS_CORE_H
#define KACCOUNTS_CORE_H

#include "kaccounts_export.h"

namespace Accounts
{
class Manager;
}

namespace KAccounts
{
/**
 * Returns a single instance of Accounts::Manager
 *
 * Always use this in your application if you need Accounts::Manager
 * as multiple managers can lead to concurrency issues
 * with the backend
 */
KACCOUNTS_EXPORT Accounts::Manager *accountsManager();
}
#endif
