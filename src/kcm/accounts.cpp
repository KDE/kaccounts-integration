/*
 *   SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "accounts.h"

#include <KLocalizedString>
#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(AccountsSettings, "kcm_kaccounts.json")

AccountsSettings::AccountsSettings(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    : KQuickAddons::ConfigModule(parent, data, args)
#else
    : KQuickConfigModule(parent, data, args)
#endif
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    setButtons(KQuickAddons::ConfigModule::NoAdditionalButton);
#else
    setButtons(KQuickConfigModule::NoAdditionalButton);
#endif
}

#include "accounts.moc"
