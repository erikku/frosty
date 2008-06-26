/******************************************************************************\
*  client/src/SkillList.h                                                      *
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

#ifndef __SkillList_h__
#define __SkillList_h__

#include "ui_SkillList.h"

#include <QtCore/QMap>
#include <QtCore/QVariant>

class SkillView;
class QListWidgetItem;

class SkillList : public QWidget
{
	Q_OBJECT

public:
	SkillList(SkillView *view = 0, QWidget *parent = 0);

public slots:
	void refresh();
	void updateFilter();
	void updateSearch();
	void deleteSkill();

protected slots:
	void ajaxResponse(const QVariant& resp);
	void handleSkillSelected();

signals:
	void addSkillRequested();
	void skillClicked(int id);

protected:
	Ui::SkillList ui;

	int mFilterID;
	int mCurrentID;

	SkillView *mSkillView;
	QVariantList mSkills;
	QListWidgetItem *mLastItem;
};

#endif // __SkillList_h__
