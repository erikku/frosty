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
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QMessageBox>

RelationList::RelationList(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	mEdit = new RelationEdit;

	setWindowModality(Qt::ApplicationModal);

	ajax::getSingletonPtr()->subscribe(this);

	connect(mEdit, SIGNAL(relationsUpdated()), this, SLOT(refresh()));
	connect(ui.closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));
	connect(ui.addButton, SIGNAL(clicked(bool)), this, SLOT(addRelation()));
	connect(ui.editButton, SIGNAL(clicked(bool)), this, SLOT(editRelation()));
	connect(ui.deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteRelation()));

	if( !settings->canDelete() )
	{
		ui.deleteButton->setVisible(false);
		ui.deleteButton->setEnabled(false);
	}

};

RelationList::~RelationList()
{
	delete mEdit;
};

void RelationList::addRelation()
{
	mEdit->showEdit(tr("Add %1").arg( windowTitle() ), mTable, -1);
};

void RelationList::editRelation()
{
	if( !ui.relationList->selectedItems().count() )
		return;

	QListWidgetItem *item = ui.relationList->selectedItems().first();
	if(!item)
		return;

	mEdit->showEdit(tr("Edit %1").arg( windowTitle() ), mTable,
		item->data(Qt::UserRole).toInt());
};

void RelationList::deleteRelation()
{
	if( !ui.relationList->selectedItems().count() )
		return;

	QListWidgetItem *item = ui.relationList->selectedItems().first();
	if(!item)
		return;

	int id = item->data(Qt::UserRole).toInt();
	if(id <= 0)
		return;

	QMessageBox::StandardButton button = QMessageBox::warning(this,
		tr("Deleting Relation"),
		tr("You are about to delete the relation '%1'. Deleting a relation "
		"could cause problems, so make sure the relation is not in use. "
		"Are you sure you want to delete this?").arg( item->text() ),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

	if(button == QMessageBox::No)
		return;

	QVariantMap where;
	where["type"] = "equals";
	where["column"] = "id";
	where["value"] = QVariant(id);

	QVariantMap action;
	action["action"] = QString("delete");
	action["table"] = QString("db_skills");
	action["where"] = QVariantList() << where;
	action["user_data"] = QString("relation_delete");

	ajax::getSingletonPtr()->request(settings->url(), action);
};

void RelationList::refresh()
{
	ui.relationList->clear();
	setEnabled(false);

	QVariantList columns;
	columns << QString("id") << QString("name_en") << QString("name_ja");

	QVariantMap orderby_name;
	orderby_name["column"] = QString("name_%1").arg(settings->lang());
	orderby_name["direction"] = "asc";

	QVariantMap orderby_id;
	orderby_id["column"] = "id";
	orderby_id["direction"] = "asc";

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_%1").arg(mTable);
	action["columns"] = columns;
	action["order_by"] = QVariantList() << orderby_name << orderby_id;
	action["user_data"] = QString("%1_cache").arg(mTable);

	ajax::getSingletonPtr()->request(settings->url(), action);
};

void RelationList::showList(const QString& windowTitle, const QString& table)
{
	setWindowTitle(windowTitle);
	mTable = table;

	refresh();
	show();
};

void RelationList::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();

	if(result.value("user_data").toString() == QString("%1_cache").arg(mTable))
	{
		QVariantList rows = result.value("rows").toList();
		for(int i = 0; i < rows.count(); i++)
		{
			QVariantMap map = rows.at(i).toMap();

			QListWidgetItem *item = new QListWidgetItem(
				map[QString("name_%1").arg(settings->lang())].toString() );
			item->setData(Qt::UserRole, map["id"]);

			ui.relationList->addItem(item);
		}

		setEnabled(true);
	}
	else if(result.value("user_data").toString() == "relation_delete")
	{
		refresh();
	}
};
