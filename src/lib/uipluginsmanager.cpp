/*
 *  SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "uipluginsmanager.h"

#include "debug.h"
#include "kaccountsuiplugin.h"

#include <KPluginMetaData>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QHash>
#include <QPluginLoader>
#include <QStringList>
#include <QWindow>

namespace KAccounts
{

class UiPluginsManagerPrivate
{
public:
    UiPluginsManagerPrivate();
    ~UiPluginsManagerPrivate();

    void loadPlugins();

    QHash<QString, KAccountsUiPlugin *> pluginsForNames;
    QHash<QString, KAccountsUiPlugin *> pluginsForServices;
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
    const auto availablePlugins = KPluginMetaData::findPlugins(QStringLiteral("kaccounts/ui"), {}, KPluginMetaData::AllowEmptyMetaData);
    for (const KPluginMetaData &plugin : availablePlugins) {
        QPluginLoader loader(plugin.fileName());

        if (!loader.load()) {
            qCWarning(KACCOUNTS_LIB_LOG) << "Could not create KAccountsUiPlugin: " << plugin.fileName();
            qCWarning(KACCOUNTS_LIB_LOG) << loader.errorString();
            continue;
        }

        if (QObject *obj = loader.instance()) {
            KAccountsUiPlugin *ui = qobject_cast<KAccountsUiPlugin *>(obj);
            if (!ui) {
                qCDebug(KACCOUNTS_LIB_LOG) << "Plugin could not be converted to an KAccountsUiPlugin";
                qCDebug(KACCOUNTS_LIB_LOG) << plugin.fileName();
                continue;
            }

            qCDebug(KACCOUNTS_LIB_LOG) << "Adding plugin" << ui << plugin.fileName();
            const QWindowList topLevelWindows = QGuiApplication::topLevelWindows();
            if (topLevelWindows.size() == 1) {
                QWindow *topLevelWindow = topLevelWindows.at(0);
                obj->setProperty("transientParent", QVariant::fromValue(topLevelWindow));
            } else {
                qCWarning(KACCOUNTS_LIB_LOG) << "Unexpected topLevelWindows found:" << topLevelWindows.size() << "please report a bug";
            }

            // When the plugin has finished building the UI, show it right away
            QObject::connect(ui, &KAccountsUiPlugin::uiReady, ui, &KAccountsUiPlugin::showNewAccountDialog, Qt::UniqueConnection);

            pluginsForNames.insert(plugin.pluginId(), ui);
            const auto services = ui->supportedServicesForConfig();
            for (const QString &service : services) {
                qCDebug(KACCOUNTS_LIB_LOG) << " Adding service" << service;
                pluginsForServices.insert(service, ui);
            }
        } else {
            qCDebug(KACCOUNTS_LIB_LOG) << "Plugin could not create instance" << plugin.fileName();
        }
    }

    pluginsLoaded = true;
}

QList<KAccountsUiPlugin *> UiPluginsManager::uiPlugins()
{
    if (!s_instance->pluginsLoaded) {
        s_instance->loadPlugins();
    }

    return s_instance->pluginsForNames.values();
}

KAccountsUiPlugin *UiPluginsManager::pluginForName(const QString &name)
{
    if (!s_instance->pluginsLoaded) {
        s_instance->loadPlugins();
    }

    return s_instance->pluginsForNames.value(name);
}

KAccountsUiPlugin *UiPluginsManager::pluginForService(const QString &service)
{
    if (!s_instance->pluginsLoaded) {
        s_instance->loadPlugins();
    }

    return s_instance->pluginsForServices.value(service);
}

};
