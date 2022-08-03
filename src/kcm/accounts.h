/*
 *   SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef ACCOUNTSSETTINGS_H
#define ACCOUNTSSETTINGS_H

#include <KQuickAddons/ConfigModule>
#include <QObject>
#include <QVariant>

class AccountsSettings : public KQuickAddons::ConfigModule
{
    Q_OBJECT

public:
    explicit AccountsSettings(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
};

#endif // ACCOUNTSSETTINGS_H
