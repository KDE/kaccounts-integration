/*
 *   SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef ACCOUNTSSETTINGS_H
#define ACCOUNTSSETTINGS_H

#include <QObject>
#include <QVariant>
#include <KQuickAddons/ConfigModule>


class AccountsSettings : public KQuickAddons::ConfigModule
{
    Q_OBJECT

    public:
        explicit AccountsSettings(QObject* parent, const QVariantList& args);
        ~AccountsSettings() override = default;
};

#endif // ACCOUNTSSETTINGS_H
