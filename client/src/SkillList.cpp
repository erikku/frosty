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

SkillList::SkillList(SkillView *view, QWidget *parent) : QWidget(parent), mFilterID(-1),
	mCurrentID(-1), mSkillView(view), mLastItem(0)
{
	ui.setupUi(this);

	ui.filterCombo->addItem( tr("All") );

	connect(ui.filterCombo, SIGNAL(currentIndexChanged(int)), this,
		SLOT(updateFilter()));

	connect(ui.searchEdit, SIGNAL(textChanged(const QString&)), this,
		SLOT(updateSearch()));

	connect(ui.skillsList, SIGNAL(itemSelectionChanged()),
		this, SLOT(handleSkillSelected()));

	connect(ui.refreshButton, SIGNAL(clicked(bool)), this, SLOT(refresh()));
	connect(ui.deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteSkill()));

	if( !settings->canDelete() )
	{
		ui.deleteButton->setVisible(false);
		ui.deleteButton->setEnabled(false);
	}

	connect(ui.addButton, SIGNAL(clicked(bool)),
		this, SIGNAL(addSkillRequested()));

	connect(ui.addButton, SIGNAL(clicked(bool)),
		ui.skillsList, SLOT(clearSelection()));

	ajax::getSingletonPtr()->subscribe(this);

	// Hide the add button unless you have permissions to edit
	ui.addButton->setVisible(false);

	{
		QVariantMap action;
		action["action"] = "auth_query_perms";
		action["user_data"] = "auth_query_perms";

		ajax::getSingletonPtr()->request(settings->url(), action);
	}

	refresh();
};

void SkillList::handleSkillSelected()
{
	if( !ui.skillsList->selectedItems().count() )
	{
		ui.deleteButton->setEnabled(false);

		return;
	}

	ui.deleteButton->setEnabled(true);

	QListWidgetItem *item = ui.skillsList->selectedItems().first();
	if(!item)
		return;

	if(mLastItem == item)
		return;

	mCurrentID = mSkills.at(
		item->data(Qt::UserRole).toInt() ).toMap().value("id").toInt();

	if( mSkillView && mSkillView->isEditing() )
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this,
			tr("Switching Skills"),
			tr("You are attemping to view another skill while editing a skill. "
			"If you continue you will lose any changes you made to the skill. "
			"Are you sure you want to continue?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

		if(button == QMessageBox::No)
		{
			bool blocked = ui.skillsList->blockSignals(true);
			ui.skillsList->clearSelection();

			if(mLastItem)
			{
				ui.skillsList->setCurrentItem(mLastItem);
				mLastItem->setSelected(true);
			}

			ui.skillsList->blockSignals(blocked);

			return;
		}
	}

	mLastItem = item;

	emit skillClicked(
		mSkills.at( item->data(Qt::UserRole).toInt() ).toMap().value("id").toInt() );
};

void SkillList::updateFilter()
{
	QComboBox *combo = ui.filterCombo;
	QListWidget *list = ui.skillsList;

	bool blocked = list->blockSignals(true);
	int index = -1;

	list->clear();

	int id = combo->itemData( combo->currentIndex() ).toInt();
	if( combo->count() > 1 )
		mFilterID = id;

	for(int i = 0; i < mSkills.count(); i++)
	{
		QVariantMap map = mSkills.at(i).toMap();
		if(id > 0 && map["expert"].toInt() != id)
			continue;

		QString icon_path = QString("icons/skills/icon_%1.png").arg(
			map["icon"].toString() );

		QListWidgetItem *item = new QListWidgetItem(QIcon(icon_path),
			map[QString("name_%1").arg(settings->lang())].toString());

		item->setData(Qt::UserRole, QVariant(i));
		list->addItem(item);

		if(map["id"].toInt() == mCurrentID)
			index = list->row(item);
	}

	if(index >= 0)
	{
		mLastItem = list->item(index);
		list->setCurrentRow(index);
		list->blockSignals(blocked);
	}
	else
	{
		list->blockSignals(blocked);
		list->clearSelection();
	}
};

void SkillList::refresh()
{
	mCurrentID = -1;
	if( ui.skillsList->selectedItems().count() )
	{
		mCurrentID = mSkills.at( ui.skillsList->selectedItems().first()->
			data(Qt::UserRole).toInt() ).toMap().value("id").toInt();
	}

	mSkills.clear();
	ui.filterCombo->clear();
	ui.filterCombo->addItem( tr("All") );

	// Experts
	{
		QVariantList columns;
		columns << QString("id") << QString("name_en") << QString("name_ja");

		QVariantMap action;
		action["action"] = QString("select");
		action["table"] = QString("db_expert");
		action["columns"] = columns;
		action["user_data"] = QString("expert_cache");

		ajax::getSingletonPtr()->request(settings->url(), action);
	}

	// Skills
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
		action["user_data"] = QString("skills");

		ajax::getSingletonPtr()->request(settings->url(), action);
	}

	setEnabled(false);
};

void SkillList::updateSearch()
{
	QString search = ui.searchEdit->text();
	QListWidget *list = ui.skillsList;

	if( search.isEmpty() )
	{
		// Show all skills
		for(int i = 0; i < list->count(); i++)
			list->item(i)->setHidden(false);

		return;
	}

	for(int i = 0; i < list->count(); i++)
	{
		QListWidgetItem *item = list->item(i);
		QVariantMap map = mSkills.at( item->data(Qt::UserRole).toInt() ).toMap();

		item->setHidden( !(map["name_en"].toString().contains(search, Qt::CaseInsensitive) ||
			map[QString("name_%1").arg(settings->lang())].toString().contains(
			search, Qt::CaseInsensitive)) );
	}
};

void SkillList::deleteSkill()
{
	if( !ui.skillsList->selectedItems().count() )
		return;

	QListWidgetItem *item = ui.skillsList->selectedItems().first();
	if(!item)
		return;

	int id = mSkills.at( item->data(Qt::UserRole).toInt() ).toMap().value("id").toInt();
	if(id <= 0)
		return;

	QMessageBox::StandardButton button = QMessageBox::warning(this,
		tr("Deleting Skill"),
		tr("You are about to delete the skill '%1'. Are you sure you want to "
		"delete this?").arg( item->text() ),
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
	action["user_data"] = QString("skill_delete");

	ajax::getSingletonPtr()->request(settings->url(), action);
};

void SkillList::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();
	if( result.contains("error") )
		return;

	QVariantList rows = result.value("rows").toList();

	if(result.value("user_data").toString() == "skills")
	{
		mSkills = rows;
	}
	else if(result.value("user_data").toString() == "skill_delete")
	{
		refresh();

		if(mSkillView)
		{
			mSkillView->cancel();
			mSkillView->clear();
		}

		return;
	}
	else if(result.value("user_data").toString() == "expert_cache")
	{
		QComboBox *combo = ui.filterCombo;

		int index = -1;
		bool blocked = combo->blockSignals(true);

		combo->clear();
		combo->addItem( tr("All") );

		for(int i = 0; i < rows.count(); i++)
		{
			QVariantMap map = rows.at(i).toMap();

			combo->addItem(
				map[QString("name_%1").arg(settings->lang())].toString(), map["id"]);

			if(map["id"].toInt() == mFilterID)
				index = i + 1;
		}

		if(index >= 0)
			combo->setCurrentIndex(index);
		else
			combo->setCurrentIndex(0);

		combo->blockSignals(blocked);
	}
	else if(result.value("user_data").toString() == "auth_query_perms")
	{
		if( result.value("perms").toMap().value("modify_db").toBool() )
			ui.addButton->setVisible(true);
	}

	if( ui.filterCombo->count() > 1 && mSkills.count() )
	{
		updateFilter();
		updateSearch();
		setEnabled(true);
	}
};
