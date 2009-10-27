/******************************************************************************\
*  client/src/PersonaWindow.cpp                                                  *
*  Copyright (C) 2008 John Eric Martin <john.eric.martin@gmail.com>            *
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

#include "PersonaWindow.h"
#include "PersonaList.h"
#include "PersonaView.h"
#include "UserList.h"
#include "Options.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>

PersonaWindow::PersonaWindow(QWidget *parent_widget) : QWidget(parent_widget)
{
	mPersonaView = new PersonaView;
	mPersonaList = new PersonaList(mPersonaView);

	QSplitter *mainSplitter = new QSplitter;
	mainSplitter->addWidget(mPersonaList);
	mainSplitter->addWidget(mPersonaView);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mainSplitter);

	connect(mPersonaList, SIGNAL(itemClicked(int)), mPersonaView, SLOT(view(int)));
	connect(mPersonaView, SIGNAL(viewChanged()), mPersonaList, SLOT(refresh()));
	connect(mPersonaList, SIGNAL(addItemRequested()), mPersonaView, SLOT(add()));
	connect(Options::getSingletonPtr(), SIGNAL(optionsChanged()),
		this, SLOT(refresh()));

	setLayout(mainLayout);
	setWindowTitle( tr("%1 - Persona List").arg(
		tr("Absolutely Frosty") ) );

	resize(800, 600);
}

void PersonaWindow::refresh()
{
	mPersonaList->refresh();
	mPersonaView->refresh();
}
