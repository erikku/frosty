/******************************************************************************\
*  client/src/SkillList.cpp                                                    *
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

#include "SkillList.h"
#include "SkillView.h"
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

SkillList::SkillList(SkillView *view,
	QWidget *parent_widget) : AjaxList(view, parent_widget)
{
	refresh();
}

QVariant SkillList::filterAction() const
{
	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja");

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_expert");
	action["columns"] = columns;
	action["user_data"] = filterUserData();

	return action;
}

QVariant SkillList::filterUserData() const
{
	return QString("expert_cache");
}

QVariant SkillList::listAction() const
{
	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja")
		<< QString("icon") << QString("expert");

	QVariantMap orderby_expert;
	orderby_expert["column"] = "expert";
	orderby_expert["direction"] = "asc";

	QVariantMap orderby_class;
	orderby_class["column"] = "class";
	orderby_class["direction"] = "asc";

	QVariantMap orderby_rank;
	orderby_rank["column"] = "rank";
	orderby_rank["direction"] = "asc";

	QVariantMap orderby_id;
	orderby_id["column"] = "id";
	orderby_id["direction"] = "asc";

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_skills");
	action["columns"] = columns;
	action["order_by"] = QVariantList() << orderby_expert << orderby_class
		<< orderby_rank << orderby_id;
	action["user_data"] = listUserData();

	return action;
}

QVariant SkillList::listUserData() const
{
	return QString("skills");
}

QString SkillList::switchTitle() const
{
	return tr("Switching Skills");
}

QString SkillList::switchMessage() const
{
	return tr("You are attemping to view another skill while editing a skill. "
			"If you continue you will lose any changes you made to the skill. "
			"Are you sure you want to continue?");
}

QString SkillList::deleteTitle() const
{
	return tr("Deleting Skill");
}

QString SkillList::deleteMessage() const
{
	return tr("You are about to delete the skill '%1'. Are you sure you want "
		"to delete this?");
}

QVariant SkillList::deleteAction(int id) const
{
	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = QVariant(id);

	QVariantMap action;
	action["action"] = QString("delete");
	action["table"] = QString("db_skills");
	action["where"] = QVariantList() << where;
	action["user_data"] = deleteUserData();

	return action;
}

QVariant SkillList::deleteUserData() const
{
	return QString("skill_delete");
}

int SkillList::filterID(const QVariantMap& map) const
{
	return map["expert"].toInt();
}

QString SkillList::itemIcon(const QVariantMap& map) const
{
	return QString("icons/skills/icon_%1.png").arg( map["icon"].toString() );
}
