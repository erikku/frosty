/******************************************************************************\
*  client/src/ItemList.cpp                                                     *
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

#include "ItemList.h"
#include "ItemView.h"
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

ItemList::ItemList(ItemView *view,
	QWidget *parent_widget) : AjaxList(view, parent_widget)
{
	refresh();
}

QVariant ItemList::filterAction() const
{
	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja");

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_item_type");
	action["columns"] = columns;
	action["user_data"] = filterUserData();

	return action;
}

QVariant ItemList::filterUserData() const
{
	return QString("expert_cache");
}

QVariant ItemList::listAction() const
{
	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja")
		<< QString("icon") << QString("type");

	QVariantMap orderby_equip;
	orderby_equip["column"] = "type";
	orderby_equip["direction"] = "asc";

	QVariantMap orderby_id;
	orderby_id["column"] = "id";
	orderby_id["direction"] = "asc";

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_items");
	action["columns"] = columns;
	action["order_by"] = QVariantList() << orderby_equip << orderby_id;
	action["user_data"] = listUserData();

	return action;
}

QVariant ItemList::listUserData() const
{
	return QString("items");
}

QString ItemList::switchTitle() const
{
	return tr("Switching Items");
}

QString ItemList::switchMessage() const
{
	return tr("You are attemping to view another item while editing a item. "
			"If you continue you will lose any changes you made to the item. "
			"Are you sure you want to continue?");
}

QString ItemList::deleteTitle() const
{
	return tr("Deleting Item");
}

QString ItemList::deleteMessage() const
{
	return tr("You are about to delete the item '%1'. Are you sure you want "
		"to delete this?");
}

QVariant ItemList::deleteAction(int id) const
{
	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = QVariant(id);

	QVariantMap action;
	action["action"] = QString("delete");
	action["table"] = QString("db_items");
	action["where"] = QVariantList() << where;
	action["user_data"] = deleteUserData();

	return action;
}

QVariant ItemList::deleteUserData() const
{
	return QString("item_delete");
}

int ItemList::filterID(const QVariantMap& map) const
{
	return map["type"].toInt();
}

QString ItemList::itemIcon(const QVariantMap& map) const
{
	QString icon_path = map["icon"].toString();
	if( icon_path.isEmpty() )
		return QString();

	return QString("icons/items/icon_%1.png").arg(icon_path);
}
