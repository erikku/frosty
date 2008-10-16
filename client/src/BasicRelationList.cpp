/******************************************************************************\
*  client/src/BasicRelationList.cpp                                            *
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

#include "BasicRelationList.h"
#include "RelationEdit.h"
#include "Settings.h"
#include "ajax.h"

BasicRelationList::BasicRelationList(RelationEdit *edit,
	QWidget *parent) : RelationList(edit, parent)
{
	ui.setupUi(this);

	connect(ui.closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));
	connect(ui.addButton, SIGNAL(clicked(bool)), this, SLOT(addRelation()));
	connect(ui.editButton, SIGNAL(clicked(bool)), this, SLOT(editRelation()));
	connect(ui.deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteRelation()));
	connect(ui.relationList, SIGNAL(itemSelectionChanged()),
		this, SLOT(itemSelectionChanged()));

	itemSelectionChanged();
	if( !settings->canDelete() )
	{
		ui.deleteButton->setVisible(false);
		ui.deleteButton->setEnabled(false);
	}
};

void BasicRelationList::itemSelectionChanged()
{
	ui.editButton->setEnabled( !ui.relationList->selectedItems().isEmpty() );
};

void BasicRelationList::editRelation(int id)
{
	Q_UNUSED(id);

	if( ui.relationList->selectedItems().isEmpty() )
		return;

	QListWidgetItem *item = ui.relationList->selectedItems().first();
	if(!item)
		return;

	RelationList::editRelation( item->data(Qt::UserRole).toInt() );
};

void BasicRelationList::deleteRelation(int id, const QString& name)
{
	Q_UNUSED(id);
	Q_UNUSED(name);

	if( ui.relationList->selectedItems().isEmpty() )
		return;

	QListWidgetItem *item = ui.relationList->selectedItems().first();
	if(!item)
		return;

	RelationList::deleteRelation( item->data(Qt::UserRole).toInt(),
		item->text() );
};

void BasicRelationList::ajaxResponse(const QVariant& resp)
{
	RelationList::ajaxResponse(resp);

	if( mTable.isEmpty() )
		return;

	QVariantMap result = resp.toMap();

	if( result.value("user_data").toString() ==
		QString("%1_relation_list").arg(mTable) )
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
};

void BasicRelationList::refresh()
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
	action["table"] = mTable;
	action["columns"] = columns;
	action["order_by"] = QVariantList() << orderby_name << orderby_id;
	action["user_data"] = QString("%1_relation_list").arg(mTable);

	ajax::getSingletonPtr()->request(settings->url(), action);
};
