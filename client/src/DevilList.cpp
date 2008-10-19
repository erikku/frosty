/******************************************************************************\
*  client/src/DevilList.cpp                                                    *
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

#include "DevilList.h"
#include "DevilView.h"
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

DevilList::DevilList(DevilView *view,
	QWidget *parent_widget) : AjaxList(view, parent_widget)
{
	refresh();
}

QVariant DevilList::filterAction() const
{
	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja");

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_genus");
	action["columns"] = columns;
	action["user_data"] = filterUserData();

	return action;
}

QVariant DevilList::filterUserData() const
{
	return QString("genus_cache");
}

QVariant DevilList::listAction() const
{
	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja")
		<< QString("icon") << QString("genus");

	QVariantMap orderby_genus;
	orderby_genus["column"] = "genus";
	orderby_genus["direction"] = "asc";

	QVariantMap orderby_level;
	orderby_level["column"] = "lvl";
	orderby_level["direction"] = "asc";

	QVariantMap orderby_id;
	orderby_id["column"] = "id";
	orderby_id["direction"] = "asc";

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_devils");
	action["columns"] = columns;
	action["order_by"] = QVariantList() << orderby_genus << orderby_level
		<< orderby_id;
	action["user_data"] = listUserData();

	return action;
}

QVariant DevilList::listUserData() const
{
	return QString("devils");
}

QString DevilList::switchTitle() const
{
	return tr("Switching Devils");
}

QString DevilList::switchMessage() const
{
	return tr("You are attemping to view another devil while editing a devil. "
			"If you continue you will lose any changes you made to the devil. "
			"Are you sure you want to continue?");
}

QString DevilList::deleteTitle() const
{
	return tr("Deleting Devil");
}

QString DevilList::deleteMessage() const
{
	return tr("You are about to delete the devil '%1'. Are you sure you want "
		"to delete this?");
}

QVariant DevilList::deleteAction(int id) const
{
	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = QVariant(id);

	QVariantMap action;
	action["action"] = QString("delete");
	action["table"] = QString("db_devils");
	action["where"] = QVariantList() << where;
	action["user_data"] = deleteUserData();

	return action;
}

QVariant DevilList::deleteUserData() const
{
	return QString("devil_delete");
}

int DevilList::filterID(const QVariantMap& map) const
{
	return map["genus"].toInt();
}

QString DevilList::itemIcon(const QVariantMap& map) const
{
	return QString("icons/devils/icon_%1.png").arg( map["icon"].toString() );
}
