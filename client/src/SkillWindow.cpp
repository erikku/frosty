/******************************************************************************\
*  client/src/SkillWindow.cpp                                                  *
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

#include "SkillWindow.h"
#include "SkillList.h"
#include "SkillView.h"
#include "UserList.h"
#include "Options.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>

SkillWindow::SkillWindow(QWidget *parent_widget) : QWidget(parent_widget)
{
	mSkillView = new SkillView;
	mSkillList = new SkillList(mSkillView);

	QSplitter *mainSplitter = new QSplitter;
	mainSplitter->addWidget(mSkillList);
	mainSplitter->addWidget(mSkillView);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mainSplitter);

	connect(mSkillList, SIGNAL(itemClicked(int)), mSkillView, SLOT(view(int)));
	connect(mSkillView, SIGNAL(viewChanged()), mSkillList, SLOT(refresh()));
	connect(mSkillList, SIGNAL(addItemRequested()), mSkillView, SLOT(add()));
	connect(Options::getSingletonPtr(), SIGNAL(optionsChanged()),
		this, SLOT(refresh()));

	setLayout(mainLayout);
	setWindowTitle( tr("%1 - Skill List").arg(
		tr("Shin Megami Tensei IMAGINE DB") ) );

	resize(800, 600);
}

void SkillWindow::refresh()
{
	mSkillList->refresh();
	mSkillView->refresh();
}
