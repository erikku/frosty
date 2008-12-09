/******************************************************************************\
*  client/src/ItemWindow.cpp                                                   *
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

#include "ItemWindow.h"
#include "ItemList.h"
#include "ItemView.h"
#include "UserList.h"
#include "Options.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>

ItemWindow::ItemWindow(QWidget *parent_widget) : QWidget(parent_widget)
{
	mItemView = new ItemView;
	mItemList = new ItemList(mItemView);

	QSplitter *mainSplitter = new QSplitter;
	mainSplitter->addWidget(mItemList);
	mainSplitter->addWidget(mItemView);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mainSplitter);

	connect(mItemList, SIGNAL(itemClicked(int)), mItemView, SLOT(view(int)));
	connect(mItemView, SIGNAL(viewChanged()), mItemList, SLOT(refresh()));
	connect(mItemList, SIGNAL(addItemRequested()), mItemView, SLOT(add()));
	connect(Options::getSingletonPtr(), SIGNAL(optionsChanged()),
		this, SLOT(refresh()));

	setLayout(mainLayout);
	setWindowTitle( tr("%1 - Item List").arg(
		tr("Shin Megami Tensei IMAGINE DB") ) );

	resize(800, 600);
}

void ItemWindow::refresh()
{
	mItemList->refresh();
	mItemView->refresh();
}
