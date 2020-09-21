/*
 *   SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "accounts.h"

#include <KLocalizedString>
#include <KAboutData>
#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(AccountsSettings, "kcm_kaccounts.json")

AccountsSettings::AccountsSettings(QObject* parent, const QVariantList& args)
    : KQuickAddons::ConfigModule(parent, args)
{
    KAboutData* about = new KAboutData(QStringLiteral("kcm_kaccounts"), i18n("Accounts"),
                                       QStringLiteral("1.0"), QString(), KAboutLicense::LGPL);
    about->addAuthor(i18n("Sebastian Kügler"), QString(), QStringLiteral("sebas@kde.org"));
    about->addAuthor(i18n("Dan Leinir Turthra Jensen"), QString(), QStringLiteral("admin@leinir.dk"), QString(), QStringLiteral("leinir"));
    setAboutData(about);
    setButtons(KQuickAddons::ConfigModule::NoAdditionalButton);
}

#include "accounts.moc"
