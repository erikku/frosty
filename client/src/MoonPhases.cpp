/******************************************************************************\
*  client/src/MoonPhases.cpp                                                   *
*  Copyright (C) 2009 John Eric Martin <john.eric.martin@gmail.com>            *
*                                                                              *
*  This program is free software; you can redistribute it and/or modify        *
*  it under the terms of the GNU General Public License version 2 as           *
*  published by the Free Software Foundation.                                  *
*                                                                              *
*  This program is distributed in the hope that it will be useful,             *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
*  GNU General Public License for more details.                                *
*                                                                              *
*  You should have received a copy of the GNU General Public License           *
*  along with this program; if not, write to the                               *
*  Free Software Foundation, Inc.,                                             *
*  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                   *
\******************************************************************************/

#include "MoonPhases.h"

MoonPhases::MoonPhases(QWidget *parent_widget) : QWidget(parent_widget)
{
	ui.setupUi(this);

	setWindowTitle( tr("%1 - Moon Phases").arg(
		tr("Absolutely Frosty") ) );

	connect(ui.allPhasesButton, SIGNAL(toggled(bool)),
		this, SLOT(updateList()));
	connect(ui.fullHalfPhasesButton, SIGNAL(toggled(bool)),
		this, SLOT(updateList()));
	connect(ui.closeButton, SIGNAL(clicked(bool)),
		this, SLOT(close()));
}

void MoonPhases::addMoonPhase(const QDateTime& start,
	const QDateTime& stop, uint moon, const QString& phase)
{
	QString text = tr("%1 @ %2 - %3").arg(phase).arg(
		start.toString("MM/dd hh:mmap") ).arg( stop.toString("hh:mmap") );

	QListWidgetItem *item = new QListWidgetItem(text);
	item->setData(Qt::UserRole, moon);

	ui.moonPhaseList->addItem(item);
}

void MoonPhases::clear()
{
	ui.moonPhaseList->clear();
}

void MoonPhases::updateList()
{
	if( ui.allPhasesButton->isChecked() )
	{
		for(int i = 0; i < ui.moonPhaseList->count(); i++)
			ui.moonPhaseList->item(i)->setHidden(false);
	}
	else
	{
		for(int i = 0; i < ui.moonPhaseList->count(); i++)
		{
			uint moon = ui.moonPhaseList->item(i)->data(Qt::UserRole).toInt();

			ui.moonPhaseList->item(i)->setHidden(moon != 0 && moon != 8);
		}
	}
}
