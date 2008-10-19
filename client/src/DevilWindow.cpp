/******************************************************************************\
*  client/src/DevilWindow.cpp                                                  *
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

#include "DevilWindow.h"
#include "DevilList.h"
#include "DevilView.h"
#include "UserList.h"
#include "Options.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>

DevilWindow::DevilWindow(QWidget *parent_widget) : QWidget(parent_widget)
{
	mDevilView = new DevilView;
	mDevilList = new DevilList(mDevilView);

	QSplitter *mainSplitter = new QSplitter;
	mainSplitter->addWidget(mDevilList);
	mainSplitter->addWidget(mDevilView);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mainSplitter);

	connect(mDevilList, SIGNAL(itemClicked(int)), mDevilView, SLOT(view(int)));
	connect(mDevilView, SIGNAL(viewChanged()), mDevilList, SLOT(refresh()));
	connect(mDevilList, SIGNAL(addItemRequested()), mDevilView, SLOT(add()));
	connect(Options::getSingletonPtr(), SIGNAL(optionsChanged()),
		this, SLOT(refresh()));

	setLayout(mainLayout);
	setWindowTitle( tr("%1 - Devil List").arg(
		tr("Shin Megami Tensei IMAGINE DB") ) );

	resize(800, 600);
}

void DevilWindow::refresh()
{
	mDevilList->refresh();
	mDevilView->refresh();
}
