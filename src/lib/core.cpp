/*
 * Copyright 2014 (C) Martin Klapetek <mklapetek@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#include "core.h"

#include <Accounts/Manager>

class CorePrivate
{
public:
    CorePrivate();
    Accounts::Manager *m_manager;
};

CorePrivate::CorePrivate()
    : m_manager(new Accounts::Manager())
{
}

Q_GLOBAL_STATIC(CorePrivate, s_instance)

Accounts::Manager *KAccounts::accountsManager()
{
    return s_instance->m_manager;
}
