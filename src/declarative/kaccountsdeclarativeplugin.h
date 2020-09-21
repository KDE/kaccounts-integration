/*
 *  SPDX-FileCopyrightText: 2015 Aleix Pol <aleixpol@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KACCOUNTSDECLARATIVEPLUGIN_H
#define KACCOUNTSDECLARATIVEPLUGIN_H

#include <QQmlExtensionPlugin>

class KAccountsDeclarativePlugin : public QQmlExtensionPlugin
{
Q_OBJECT
Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char* uri) override;
};

#endif // KACCOUNTSDECLARATIVEPLUGIN_H
