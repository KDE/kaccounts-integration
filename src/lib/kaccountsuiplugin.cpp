/*
 * SPDX-FileCopyrightText: 2014 Martin Klapetek <mklapetek@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#include "kaccountsuiplugin.h"

#include <QWindow>
#include <QVariant>

KAccountsUiPlugin::KAccountsUiPlugin(QObject *parent)
     : QObject(parent)
{
}

KAccountsUiPlugin::~KAccountsUiPlugin()
{
}

QWindow* KAccountsUiPlugin::transientParent() const
{
    return property("transientParent").value<QWindow*>();
}

#include "moc_kaccountsuiplugin.cpp"
