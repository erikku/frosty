/******************************************************************************\
*  client/src/RelationList.cpp                                                 *
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

#include "RelationList.h"
#include "RelationEdit.h"

#include "ajax.h"

#include <QtGui/QMessageBox>

RelationList::RelationList(RelationEdit *edit,
	QWidget *parent_widget) : QDialog(parent_widget)
{
	Q_ASSERT(edit);
	mEdit = edit;
	mEdit->setParent(this, mEdit->windowFlags());

	setWindowModality(Qt::WindowModal);

	ajax::getSingletonPtr()->subscribe(this);

	connect(mEdit, SIGNAL(relationsUpdated()), this, SLOT(refresh()));
	connect(mEdit, SIGNAL(relationsUpdated()), this, SIGNAL(relationsUpdated()));
}

RelationList::~RelationList()
{
	delete mEdit;
}

void RelationList::addRelation()
{
	mEdit->showEdit(tr("Add %1").arg( windowTitle() ), mTable, -1);
}

void RelationList::editRelation(int id)
{
	Q_ASSERT(id > 0);

	mEdit->showEdit(tr("Edit %1").arg( windowTitle() ), mTable, id);
}

void RelationList::deleteRelation(int id, const QString& name)
{
	Q_ASSERT(id > 0);

	QMessageBox::StandardButton button = QMessageBox::warning(this,
		tr("Deleting Relation"),
		tr("You are about to delete the relation '%1'. Deleting a relation "
		"could cause problems, so make sure the relation is not in use. "
		"Are you sure you want to delete this?").arg(name),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

	if(button == QMessageBox::No)
		return;

	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = QVariant(id);

	QVariantMap action;
	action["action"] = QString("delete");
	action["table"] = mTable;
	action["where"] = QVariantList() << where;
	action["user_data"] = QString("%1_relation_delete").arg(mTable);

	ajax::getSingletonPtr()->request(action);
}

void RelationList::showList(const QString& window_title, const QString& table)
{
	setWindowTitle(window_title);
	mTable = table;

	refresh();
	show();
}

void RelationList::ajaxResponse(const QVariantMap& resp,
	const QString& user_data)
{
	Q_UNUSED(resp);

	if( mTable.isEmpty() )
		return;

	if(user_data == QString("%1_relation_delete").arg(mTable))
		refresh();
}
