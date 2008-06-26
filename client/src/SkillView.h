/******************************************************************************\
*  client/src/SkillView.h                                                      *
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

#ifndef __SkillView_h__
#define __SkillView_h__

#include "ui_SkillView.h"

#include <QtCore/QVariant>
#include <QtCore/QMap>

class IconSelect;
class RelationList;

class SkillView : public QWidget
{
	Q_OBJECT

public:
	SkillView(QWidget *parent = 0);

	void setPreserveEdit(bool preserve);
	bool isEditing() const;

public slots:
	void edit();
	void clear();
	void cancel();
	void refresh();
	void addSkill();
	void updateSkill();
	void updateCombo();
	void showIconSelect();
	void view(int id, bool switchView = true);

	void editType();
	void editCategory();
	void editAffinity();
	void editExpert();

private slots:
	void costEditChanged();
	void costEditUpdate();

signals:
	void skillChanged();

protected slots:
	void ajaxResponse(const QVariant& resp);
	void updateIcon(const QString& path, const QString& value);

protected:
	void darkenWidget(QWidget *widget);

	Ui::SkillView ui;

	int mID;
	int mCostHP, mCostMP, mCostMAG;

	IconSelect *mIconSelect;
	RelationList *mRelationList;

	bool mPreserveEdit;
	bool mExpertGood, mAffinityGood, mCategoryGood, mActionTypeGood;
	int mLastExpertID, mLastActionTypeID, mLastCategoryID, mLastAffinityID;

	QVariantMap mSleepingResponse;
	QMap<int, QVariantMap> mExpertCache;
	QMap<int, QVariantMap> mAffinityCache;
	QMap<int, QVariantMap> mCategoryCache;
	QMap<int, QVariantMap> mActionTypeCache;
};

#endif // __SkillView_h__
