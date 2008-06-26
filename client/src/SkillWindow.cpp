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
#include "ajax.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>

SkillWindow::SkillWindow(QWidget *parent) : QWidget(parent), mUserList(0)
{
	mSkillView = new SkillView;
	mSkillList = new SkillList(mSkillView);

	mOptions = new Options;

	QAction *action = new QAction(tr("&Options..."), 0);
	action->setShortcut( QKeySequence("Ctrl+O") );
	addAction(action);

	connect(action, SIGNAL(triggered(bool)), mOptions, SLOT(loadSettings()));

	action = new QAction(tr("AJAX &Log..."), 0);
	action->setShortcut( QKeySequence("Ctrl+L") );
	addAction(action);

	connect(action, SIGNAL(triggered(bool)),
		ajax::getSingletonPtr(), SLOT(showLog()));

	QSplitter *mainSplitter = new QSplitter;
	mainSplitter->addWidget(mSkillList);
	mainSplitter->addWidget(mSkillView);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(mainSplitter);

	connect(mSkillList, SIGNAL(skillClicked(int)), mSkillView, SLOT(view(int)));
	connect(mSkillView, SIGNAL(skillChanged()), mSkillList, SLOT(refresh()));
	connect(mSkillList, SIGNAL(addSkillRequested()), mSkillView, SLOT(addSkill()));
	connect(mOptions, SIGNAL(optionsChanged()), this, SLOT(refresh()));

	setLayout(mainLayout);
	setWindowTitle( tr("%1 - Skill List").arg(
		tr("Shin Megami Tensei IMAGINE DB") ) );

	resize(800, 600);

	ajax::getSingletonPtr()->subscribe(this);
};

void SkillWindow::refresh()
{
	mSkillList->refresh();
	mSkillView->refresh();
};

void SkillWindow::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();
	if( result.contains("error") )
		return;

	if(result.value("user_data").toString() == "auth_query_perms")
	{
		if( result.value("perms").toMap().value("admin").toBool() )
		{
			mUserList = new UserList;

			QAction *action = new QAction(tr("&User List..."), 0);
			action->setShortcut( QKeySequence("Ctrl+U") );
			addAction(action);

			connect(action, SIGNAL(triggered(bool)), mUserList, SLOT(show()));
			connect(action, SIGNAL(triggered(bool)), mUserList, SLOT(refresh()));
		}
	}
};
