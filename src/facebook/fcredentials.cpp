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

#include "fcredentials.h"
#include "facebook.h"

FCredentials::FCredentials(FacebookWizard* parent) : QWizardPage(parent)
{
    setupUi(this);
    googleIcon->setPixmap(QIcon::fromTheme("im-facebook").pixmap(32, 32));

    m_wizard = parent;
}

FCredentials::~FCredentials()
{

}

bool FCredentials::validatePage()
{
    if (email->text().isEmpty() || password->text().isEmpty()) {
        return false;
    }

    m_wizard->setUsername(email->text());
    m_wizard->setPassword(password->text());
    return true;
}
