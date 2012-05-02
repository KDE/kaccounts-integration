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

#include "oremovefile.h"

#include <QFile>

#include <KDirNotify>

ORemoveFile::ORemoveFile(KConfigGroup group, QObject* parent)
 : KJob(parent)
 , m_config(group)
{
    setObjectName(m_config.name());
    setProperty("type", QVariant::fromValue(m_config.readEntry("type")));
}

ORemoveFile::~ORemoveFile()
{

}

void ORemoveFile::start()
{
    QMetaObject::invokeMethod(this, "removeFile", Qt::QueuedConnection);
}

void ORemoveFile::removeFile()
{
    QFile::remove(m_config.group("private").readEntry("fileDesktop"));
    org::kde::KDirNotify::emitFilesAdded( "remote:/" );
    m_config.group("services").writeEntry("File", 0);
    emitResult();
}
