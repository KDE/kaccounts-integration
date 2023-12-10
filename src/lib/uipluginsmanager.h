/*
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef UIPLUGINSMANAGER_H
#define UIPLUGINSMANAGER_H

#include <QList>

namespace KAccounts
{
class KAccountsUiPlugin;
class UiPluginsManager
{
public:
    static QList<KAccountsUiPlugin *> uiPlugins();
    static KAccountsUiPlugin *pluginForService(const QString &service);
    static KAccountsUiPlugin *pluginForName(const QString &name);
};

};

#endif // UIPLUGINSMANAGER_H
