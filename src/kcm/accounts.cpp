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
    : KQuickAddons::ConfigModule(parent, data, args)
{
    setButtons(KQuickAddons::ConfigModule::NoAdditionalButton);
}

#include "accounts.moc"
