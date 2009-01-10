/******************************************************************************\
*  client/src/TraitList.cpp                                                    *
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

#include "TraitList.h"
#include "TraitView.h"
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

TraitList::TraitList(TraitView *view,
	QWidget *parent_widget) : AjaxList(view, parent_widget)
{
	refresh();
}

QVariant TraitList::filterAction() const
{
	return QVariant();
}

QVariant TraitList::filterUserData() const
{
	return QString();
}

QVariant TraitList::listAction() const
{
	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja");

	QVariantMap orderby_id;
	orderby_id["column"] = "id";
	orderby_id["direction"] = "asc";

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_traits");
	action["columns"] = columns;
	action["order_by"] = QVariantList() << orderby_id;
	action["user_data"] = listUserData();

	return action;
}

QVariant TraitList::listUserData() const
{
	return QString("traits");
}

QString TraitList::switchTitle() const
{
	return tr("Switching Traits");
}

QString TraitList::switchMessage() const
{
	return tr("You are attemping to view another trait while editing a trait. "
			"If you continue you will lose any changes you made to the trait. "
			"Are you sure you want to continue?");
}

QString TraitList::deleteTitle() const
{
	return tr("Deleting Trait");
}

QString TraitList::deleteMessage() const
{
	return tr("You are about to delete the trait '%1'. Are you sure you want "
		"to delete this?");
}

QVariant TraitList::deleteAction(int id) const
{
	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = QVariant(id);

	QVariantMap action;
	action["action"] = QString("delete");
	action["table"] = QString("db_traits");
	action["where"] = QVariantList() << where;
	action["user_data"] = deleteUserData();

	return action;
}

QVariant TraitList::deleteUserData() const
{
	return QString("trait_delete");
}

int TraitList::filterID(const QVariantMap& map) const
{
	Q_UNUSED(map);

	return 0;
}

QString TraitList::itemIcon(const QVariantMap& map) const
{
	Q_UNUSED(map);

	return QString();
}

bool TraitList::hasIcon() const
{
	return false;
}
