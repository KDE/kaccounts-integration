/*************************************************************************************
 *  Copyright (C) 2015 by Martin Klapetek <mklapetek@kde.org>                        *
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

#include "uipluginsmanager.h"

#include "kaccountsuiplugin.h"

#include <QHash>
#include <QStringList>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include <QGuiApplication>
#include <QWindow>

using namespace KAccounts;

class UiPluginsManagerPrivate
{
public:
    UiPluginsManagerPrivate();
    ~UiPluginsManagerPrivate();

    void loadPlugins();

    QHash<QString, KAccountsUiPlugin*> pluginsForNames;
    QHash<QString, KAccountsUiPlugin*> pluginsForServices;
    bool pluginsLoaded;

};

Q_GLOBAL_STATIC(UiPluginsManagerPrivate, s_instance)

UiPluginsManagerPrivate::UiPluginsManagerPrivate()
    : pluginsLoaded(false)
{
}

UiPluginsManagerPrivate::~UiPluginsManagerPrivate()
{
    qDeleteAll(pluginsForNames.values());
}

void UiPluginsManagerPrivate::loadPlugins()
{
    QString pluginPath;

    const QStringList paths = QCoreApplication::libraryPaths();
    for (const QString &libraryPath : paths) {
        QString path(libraryPath + QStringLiteral("/kaccounts/ui"));
        QDir dir(path);

        if (!dir.exists()) {
            continue;
        }

        const QStringList entryList = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        for (const QString &fileName : entryList) {
            QPluginLoader loader(dir.absoluteFilePath(fileName));

            if (!loader.load()) {
                qWarning() << "Could not create KAccountsUiPlugin: " << pluginPath;
                qWarning() << loader.errorString();
                continue;
            }

            QObject *obj = loader.instance();
            if (obj) {
                KAccountsUiPlugin *ui = qobject_cast<KAccountsUiPlugin*>(obj);
                if (!ui) {
                    qDebug() << "Plugin could not be converted to an KAccountsUiPlugin";
                    qDebug() << pluginPath;
                    continue;
                }

                qDebug() << "Adding plugin" << ui << fileName;
                const QWindowList topLevelWindows = QGuiApplication::topLevelWindows();
                if (topLevelWindows.size() == 1) {
                    QWindow *topLevelWindow = topLevelWindows.at(0);
                    obj->setProperty("transientParent", QVariant::fromValue(topLevelWindow));
                } else {
                    qWarning() << "Unexpected topLevelWindows found:" << topLevelWindows.size() << "please report a bug";
                }

                // When the plugin has finished building the UI, show it right away
                QObject::connect(ui, &KAccountsUiPlugin::uiReady, ui, &KAccountsUiPlugin::showNewAccountDialog, Qt::UniqueConnection);

                pluginsForNames.insert(fileName, ui);
                const auto services = ui->supportedServicesForConfig();
                for (const QString &service : services) {
                    qDebug() << " Adding service" << service;
                    pluginsForServices.insert(service, ui);
                }
            } else {
                qDebug() << "Plugin could not creaate instance" << pluginPath;
            }

        }
    }

    pluginsLoaded = true;
}


QList<KAccountsUiPlugin*> UiPluginsManager::uiPlugins()
{
    if (!s_instance->pluginsLoaded) {
        s_instance->loadPlugins();
    }

    return s_instance->pluginsForNames.values();
}

KAccountsUiPlugin* UiPluginsManager::pluginForName(const QString &name)
{
    if (!s_instance->pluginsLoaded) {
        s_instance->loadPlugins();
    }

    return s_instance->pluginsForNames.value(name + QStringLiteral(".so"));
}

KAccountsUiPlugin* UiPluginsManager::pluginForService(const QString &service)
{
    if (!s_instance->pluginsLoaded) {
        s_instance->loadPlugins();
    }

    return s_instance->pluginsForServices.value(service);
}
