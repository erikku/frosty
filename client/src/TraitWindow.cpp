/******************************************************************************\
*  client/src/TraitWindow.cpp                                                  *
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

#include "TraitWindow.h"
#include "TraitList.h"
#include "TraitView.h"
#include "UserList.h"
#include "Options.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>

TraitWindow::TraitWindow(QWidget *parent_widget) : QWidget(parent_widget)
{
	mTraitView = new TraitView;
	mTraitList = new TraitList(mTraitView);

	QSplitter *mainSplitter = new QSplitter;
	mainSplitter->addWidget(mTraitList);
	mainSplitter->addWidget(mTraitView);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mainSplitter);

	connect(mTraitList, SIGNAL(itemClicked(int)), mTraitView, SLOT(view(int)));
	connect(mTraitView, SIGNAL(viewChanged()), mTraitList, SLOT(refresh()));
	connect(mTraitList, SIGNAL(addItemRequested()), mTraitView, SLOT(add()));
	connect(Options::getSingletonPtr(), SIGNAL(optionsChanged()),
		this, SLOT(refresh()));

	setLayout(mainLayout);
	setWindowTitle( tr("%1 - Trait List").arg(
		tr("Absolutely Frosty") ) );

	resize(800, 600);
}

void TraitWindow::refresh()
{
	mTraitList->refresh();
	mTraitView->refresh();
}
