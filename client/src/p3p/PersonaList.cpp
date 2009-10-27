/******************************************************************************\
*  client/src/PersonaList.cpp                                                    *
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

#include "PersonaList.h"
#include "PersonaView.h"
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

PersonaList::PersonaList(PersonaView *view,
	QWidget *parent_widget) : AjaxList(view, parent_widget)
{
	refresh();
}

QVariant PersonaList::filterAction() const
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

QVariant PersonaList::filterUserData() const
{
	return QString("expert_cache");
}

QVariant PersonaList::listAction() const
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

QVariant PersonaList::listUserData() const
{
	return QString("personae");
}

QString PersonaList::switchTitle() const
{
	return tr("Switching Persona");
}

QString PersonaList::switchMessage() const
{
	return tr("You are attemping to view another persona while editing a "
		"persona. If you continue you will lose any changes you made to this "
		"persona. Are you sure you want to continue?");
}

QString PersonaList::deleteTitle() const
{
	return tr("Deleting Persona");
}

QString PersonaList::deleteMessage() const
{
	return tr("You are about to delete the persona '%1'. Are you sure you want "
		"to delete this?");
}

QVariant PersonaList::deleteAction(int id) const
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

QVariant PersonaList::deleteUserData() const
{
	return QString("persona_delete");
}

int PersonaList::filterID(const QVariantMap& map) const
{
	return map["expert"].toInt();
}

QString PersonaList::itemIcon(const QVariantMap& map) const
{
	QString icon_path = map["icon"].toString();
	if( icon_path.isEmpty() )
		return QString();

	return QString("icons/skills/icon_%1.png").arg(icon_path);
}
