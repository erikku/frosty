/******************************************************************************\
*  client/src/AddDevil.cpp                                                     *
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

#include "AddDevil.h"
#include "Settings.h"
#include "DevilCache.h"
#include "ajax.h"

AddDevil::AddDevil(QWidget *parent_widget) : QDialog(parent_widget), mSlot(-1)
{
	ui.setupUi(this);
	setEnabled(false);

	setWindowTitle( tr("%1 - Add Devil").arg(
		tr("Absolutely Frosty") ) );

	ui.devilList->setSortingEnabled(true);

	connect(ui.searchEdit, SIGNAL(textChanged(const QString&)),
		this, SLOT(updateSearch()));

	connect(ui.devilList, SIGNAL(itemSelectionChanged()),
		this, SLOT(handleItemSelected()));

	connect(ui.cancelButton, SIGNAL(clicked(bool)),
		this, SLOT(close()));

	connect(ui.addButton, SIGNAL(clicked(bool)),
		this, SLOT(addDevil()));

	setWindowModality(Qt::WindowModal);

	ajax::getSingletonPtr()->subscribe(this);

	{
		QVariantMap action;
		action["action"] = "select";
		action["user_data"] = "devil_add_cache";
		action["table"] = "db_devils";
		action["columns"] = QVariantList() << "id" << "icon"
			<< "name_en" << "name_ja";

		ajax::getSingletonPtr()->request(settings->url(), action);
	}

	handleItemSelected();
}

void AddDevil::addDevil()
{
	QList<QListWidgetItem*> items = ui.devilList->selectedItems();
	if( items.isEmpty() )
		return;

	QListWidgetItem *item = items.first();
	if( item->isHidden() )
		return;

	QVariantMap item_data = item->data(Qt::UserRole).toMap();

	int devil_id = item_data.value("id").toInt();

	QVariantMap devil_data = DevilCache::getSingletonPtr()->devilByID(devil_id);
	QVariantList default_skills = devil_data.value("skills").toList();

	QVariantList skills;
	foreach(QVariant default_skill, default_skills)
	{
		QVariantMap skill = default_skill.toMap();
		if(skill.value("lvl").toInt() == -1)
			skills << skill.value("skill");
	}

	QVariantMap blank_skill;
	blank_skill["id"] = -1;
	blank_skill["lvl"] = -1;

	while(skills.count() < 8)
		skills << blank_skill;

	// Process the devil data
	QVariantMap devil;
	devil["id"] = devil_id;
	devil["lvl"] = devil_data.value("lvl");
	devil["skills"] = skills;
	devil["inherited_skills"] = QVariantList();
	devil["parents"] = QVariantList();

	emit devilSelected(mSlot, devil);

	close();
}

void AddDevil::add(int slot)
{
	mSlot = slot;

	show();
}

void AddDevil::updateSearch()
{
	QString search = ui.searchEdit->text();
	QListWidget *list = ui.devilList;

	if( search.isEmpty() )
	{
		// Show all devils
		for(int i = 0; i < list->count(); i++)
			list->item(i)->setHidden(false);

		handleItemSelected();

		return;
	}

	for(int i = 0; i < list->count(); i++)
	{
		QListWidgetItem *item = list->item(i);
		QVariantMap map = item->data(Qt::UserRole).toMap();

		item->setHidden( !itemMatches(map, search) );
	}

	handleItemSelected();
}

void AddDevil::handleItemSelected()
{
	QList<QListWidgetItem*> items = ui.devilList->selectedItems();
	if( items.isEmpty() )
		ui.addButton->setEnabled(false);
	else if( items.first()->isHidden() )
		ui.addButton->setEnabled(false);
	else
		ui.addButton->setEnabled(true);
}

void AddDevil::ajaxResponse(const QVariant& resp)
{
	QVariantMap res = resp.toMap();

	if(res.value("user_data").toString() != "devil_add_cache")
		return;

	ui.devilList->clear();

	QVariantList devils = res.value("rows").toList();

	DevilCache *cache = DevilCache::getSingletonPtr();

	QListIterator<QVariant> it(devils);
	while( it.hasNext() )
	{
		QVariantMap devil = it.next().toMap();

		QIcon icon( QString("icons/devils/icon_%1.png").arg(
			devil.value("icon").toString() ) );
		QString name_column = QString("name_%1").arg(settings->lang());

		QListWidgetItem *item = new QListWidgetItem(icon,
			devil.value(name_column).toString());
		item->setData(Qt::UserRole, devil);
		item->setToolTip( cache->devilToolTip(devil) );

		ui.devilList->addItem(item);
	}

	setEnabled(true);
}

bool AddDevil::itemMatches(const QVariantMap& map, const QString& search)
{
	return (map["name_en"].toString().contains(search, Qt::CaseInsensitive) ||
		map["name_ja"].toString().contains(search, Qt::CaseInsensitive));
}
