/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#ifndef REMOVENETATTACHJOB_H
#define REMOVENETATTACHJOB_H

#include <KJob>
#include <KConfigGroup>

namespace KWallet {
    class Wallet;
}
class RemoveNetAttachJob : public KJob
{
    Q_OBJECT

public:
    explicit RemoveNetAttachJob(QObject *parent);
    virtual ~RemoveNetAttachJob();

    void start() override;

    QString uniqueId() const;
    void setUniqueId(const QString &uniqueId);

private Q_SLOTS:
    void removeNetAttach();
    void walletOpened(bool opened);
    void deleteDesktopFile();

private:
    QString m_uniqueId;

    KWallet::Wallet *m_wallet = nullptr;
};

#endif //REMOVENETATTACHJOB_H
