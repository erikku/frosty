/******************************************************************************\
*  client/src/MashouList.cpp                                                   *
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

#include "MashouList.h"
#include "MashouView.h"
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

MashouList::MashouList(MashouView *view,
	QWidget *parent_widget) : AjaxList(view, parent_widget)
{
	refresh();
}

QVariant MashouList::filterAction() const
{
	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja");

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_equip_type");
	action["columns"] = columns;
	action["user_data"] = filterUserData();

	return action;
}

QVariant MashouList::filterUserData() const
{
	return QString("equip_type_cache");
}

QVariant MashouList::listAction() const
{
	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja")
		<< QString("icon") << QString("equip_type");

	QVariantMap orderby_equip_type;
	orderby_equip_type["column"] = "equip_type";
	orderby_equip_type["direction"] = "asc";

	QVariantMap orderby_id;
	orderby_id["column"] = "id";
	orderby_id["direction"] = "asc";

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_mashou");
	action["columns"] = columns;
	action["order_by"] = QVariantList() << orderby_equip_type << orderby_id;
	action["user_data"] = listUserData();

	return action;
}

QVariant MashouList::listUserData() const
{
	return QString("mashou_list");
}

QString MashouList::switchTitle() const
{
	return tr("Switching Mashou");
}

QString MashouList::switchMessage() const
{
	return tr("You are attemping to view another mashou while editing a "
		"mashou. If you continue you will lose any changes you made to the "
		"mashou. Are you sure you want to continue?");
}

QString MashouList::deleteTitle() const
{
	return tr("Deleting Mashou");
}

QString MashouList::deleteMessage() const
{
	return tr("You are about to delete the mashou '%1'. Are you sure you want "
		"to delete this?");
}

QVariant MashouList::deleteAction(int id) const
{
	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = QVariant(id);

	QVariantMap action;
	action["action"] = QString("delete");
	action["table"] = QString("db_mashou");
	action["where"] = QVariantList() << where;
	action["user_data"] = deleteUserData();

	return action;
}

QVariant MashouList::deleteUserData() const
{
	return QString("mashou_delete");
}

int MashouList::filterID(const QVariantMap& map) const
{
	return map["equip_type"].toInt();
}

QString MashouList::itemIcon(const QVariantMap& map) const
{
	QString icon_path = map["icon"].toString();
	if( icon_path.isEmpty() )
		return QString();

	return QString("icons/mashou/icon_%1.png").arg(icon_path);
}
