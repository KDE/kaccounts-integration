/***************************************************************************
 *                                                                         *
 *   Copyright 2019 Nicolas Fella <nicolas.fella@gmx.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

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
