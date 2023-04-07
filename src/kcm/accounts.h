/*
 *   SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef ACCOUNTSSETTINGS_H
#define ACCOUNTSSETTINGS_H

#include <QObject>
#include <QVariant>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <KQuickAddons/ConfigModule>
#else
#include <KQuickConfigModule>
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
class AccountsSettings : public KQuickAddons::ConfigModule
#else
class AccountsSettings : public KQuickConfigModule
#endif
{
    Q_OBJECT

public:
    explicit AccountsSettings(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
};

#endif // ACCOUNTSSETTINGS_H
