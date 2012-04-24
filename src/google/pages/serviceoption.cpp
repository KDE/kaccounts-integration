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

#include "serviceoption.h"

#include <kpixmapsequenceoverlaypainter.h>
#include <QDebug>
ServiceOption::ServiceOption(const QString& name, const QString& displayText, QWidget* parent)
 : QWidget(parent)
 , m_binaryMode(false)
{
    setupUi(this);

    setObjectName(name);
    checkLabel->setText(displayText);

    working->setVisible(false);
    working->setMinimumSize(checkBox->sizeHint());

    KPixmapSequenceOverlayPainter *painter = new KPixmapSequenceOverlayPainter(this);
    painter->setWidget(working);
    painter->start();

    connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(setToggled(bool)));
}

bool ServiceOption::isChecked()
{
    return checkBox->isChecked();
}

void ServiceOption::setToggled(bool checked)
{
    const QCheckBox *checkbox = qobject_cast< const QCheckBox* >(sender());
    if (!m_binaryMode) {
        setStatus(2);
    }

    Q_EMIT toggled(objectName(), checked);
}

void ServiceOption::setStatus(int status )
{
    switch(status) {
        case 0:
            working->setVisible(false);
            checkBox->setVisible(true);
            checkBox->setChecked(false);
            break;
        case 1:
            working->setVisible(false);
            checkBox->setVisible(true);
            checkBox->setChecked(true);
            break;
        case 2:
            checkBox->setVisible(false);
            working->setVisible(true);
            break;
    }
}

void ServiceOption::setBinary(bool binary)
{
    m_binaryMode = binary;
}