/******************************************************************************\
*  client/src/MashouWindow.cpp                                                 *
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

#include "MashouWindow.h"
#include "MashouList.h"
#include "MashouView.h"
#include "UserList.h"
#include "Options.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>

MashouWindow::MashouWindow(QWidget *parent_widget) : QWidget(parent_widget)
{
	mMashouView = new MashouView;
	mMashouList = new MashouList(mMashouView);

	QSplitter *mainSplitter = new QSplitter;
	mainSplitter->addWidget(mMashouList);
	mainSplitter->addWidget(mMashouView);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mainSplitter);

	connect(mMashouList, SIGNAL(itemClicked(int)), mMashouView, SLOT(view(int)));
	connect(mMashouView, SIGNAL(viewChanged()), mMashouList, SLOT(refresh()));
	connect(mMashouList, SIGNAL(addItemRequested()), mMashouView, SLOT(add()));
	connect(Options::getSingletonPtr(), SIGNAL(optionsChanged()),
		this, SLOT(refresh()));

	setLayout(mainLayout);
	setWindowTitle( tr("%1 - Mashou List").arg(
		tr("Absolutely Frosty") ) );

	resize(800, 600);
}

void MashouWindow::refresh()
{
	mMashouList->refresh();
	mMashouView->refresh();
}
