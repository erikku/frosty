/******************************************************************************\
*  client/src/AjaxList.cpp                                                     *
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

#include "AjaxList.h"
#include "AjaxView.h"
#include "Settings.h"
#include "ajax.h"

#include <QtCore/QFileInfo>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

AjaxList::AjaxList(AjaxView *view,
	QWidget *parent_widget) : QWidget(parent_widget), mFilterID(-1),
	mCurrentID(-1), mDataLoaded(-1), mAjaxView(view), mLastItem(0)
{
	ui.setupUi(this);

	ui.filterCombo->addItem( tr("All") );

	connect(ui.filterCombo, SIGNAL(currentIndexChanged(int)), this,
		SLOT(updateFilter()));

	connect(ui.searchEdit, SIGNAL(textChanged(const QString&)),
		this, SLOT(updateSearch()));

	connect(ui.ajaxList, SIGNAL(itemSelectionChanged()),
		this, SLOT(handleItemSelected()));

	connect(ui.refreshButton, SIGNAL(clicked(bool)), this, SLOT(refresh()));
	connect(ui.deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteAjax()));

	connect(view, SIGNAL(itemInserted(int)),
		this, SLOT(handleItemInserted(int)));

	ui.imeButton->setChecked( ui.searchEdit->romajiMode() );
	connect(ui.imeButton, SIGNAL(toggled(bool)),
		ui.searchEdit, SLOT(setRomajiMode(bool)));

	if( !settings->canDelete() )
	{
		ui.deleteButton->setVisible(false);
		ui.deleteButton->setEnabled(false);
	}

	connect(ui.addButton, SIGNAL(clicked(bool)),
		this, SIGNAL(addItemRequested()));

	connect(ui.addButton, SIGNAL(clicked(bool)),
		ui.ajaxList, SLOT(clearSelection()));

	ajax::getSingletonPtr()->subscribe(this);

	// Hide the add button unless you have permissions to edit
	ui.addButton->setVisible(false);

	{
		QVariantMap action;
		action["action"] = "auth_query_perms";
		action["user_data"] = "auth_query_perms";

		ajax::getSingletonPtr()->request(settings->url(), action);
	}
}

void AjaxList::handleItemInserted(int id)
{
	if( !sender()->inherits("AjaxView") )
		return;

	mCurrentID = id;
}

void AjaxList::handleItemSelected()
{
	if( !ui.ajaxList->selectedItems().count() )
	{
		ui.deleteButton->setEnabled(false);

		return;
	}

	ui.deleteButton->setEnabled(true);

	QListWidgetItem *item = ui.ajaxList->selectedItems().first();
	if(!item)
		return;

	if(mLastItem == item)
		return;

	mCurrentID = itemID( mItems.at(
		item->data(Qt::UserRole).toInt() ).toMap() );

	if( mAjaxView && mAjaxView->isEditing() )
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this,
			switchTitle(), switchMessage(),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

		if(button == QMessageBox::No)
		{
			bool blocked = ui.ajaxList->blockSignals(true);
			ui.ajaxList->clearSelection();

			if(mLastItem)
			{
				ui.ajaxList->setCurrentItem(mLastItem);
				mLastItem->setSelected(true);
			}

			ui.ajaxList->blockSignals(blocked);

			return;
		}
	}

	mLastItem = item;

	emit itemClicked(
		itemID( mItems.at( item->data(Qt::UserRole).toInt() ).toMap() ) );
}

void AjaxList::updateFilter()
{
	QComboBox *combo = ui.filterCombo;
	QListWidget *list = ui.ajaxList;

	bool blocked = list->blockSignals(true);
	int index = -1;

	list->clear();

	int id = combo->itemData( combo->currentIndex() ).toInt();
	if( combo->count() > 1 )
		mFilterID = id;

	for(int i = 0; i < mItems.count(); i++)
	{
		QVariantMap map = mItems.at(i).toMap();
		if(id > 0 && filterID(map) != id)
			continue;

		QString icon_path = itemIcon(map);
		if( icon_path.isEmpty() )
			icon_path = ":/blank.png";
		else if( !QFileInfo(icon_path).exists() )
			icon_path = ":/missing.png";


		QListWidgetItem *item;
		if( icon_path.isEmpty() )
			item = new QListWidgetItem( itemText(map) );
		else
			item = new QListWidgetItem(QIcon(icon_path), itemText(map) );

		item->setData(Qt::UserRole, QVariant(i));
		list->addItem(item);

		if(itemID(map) == mCurrentID)
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

	updateSearch();
}

void AjaxList::refresh()
{
	mCurrentID = -1;
	if( ui.ajaxList->selectedItems().count() )
	{
		mCurrentID = itemID( mItems.at( ui.ajaxList->selectedItems().first()->
			data(Qt::UserRole).toInt() ).toMap() );
	}

	mItems.clear();
	mDataLoaded = -1;
	ui.filterCombo->clear();
	ui.filterCombo->addItem( tr("All") );

	QVariant filter_action = filterAction();
	if( !filter_action.isNull() )
		ajax::getSingletonPtr()->request(settings->url(), filter_action);
	else
		mDataLoaded++;

	ajax::getSingletonPtr()->request(settings->url(), listAction());

	setEnabled(false);
}

void AjaxList::updateSearch()
{
	QString search = ui.searchEdit->text();
	QListWidget *list = ui.ajaxList;

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
		QVariantMap map = mItems.at( item->data(Qt::UserRole).toInt() ).toMap();

		item->setHidden( !itemMatches(map, search) );
	}
}

void AjaxList::deleteAjax()
{
	if( !ui.ajaxList->selectedItems().count() )
		return;

	QListWidgetItem *item = ui.ajaxList->selectedItems().first();
	if(!item)
		return;

	int id = itemID( mItems.at( item->data(Qt::UserRole).toInt() ).toMap() );
	if(id <= 0)
		return;

	QMessageBox::StandardButton button = QMessageBox::warning(this,
		deleteTitle(), deleteMessage().arg( item->text() ),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

	if(button == QMessageBox::No)
		return;

	ajax::getSingletonPtr()->request(settings->url(), deleteAction(id));

	QVariantList custom_actions = mAjaxView->createDeleteActions(id);
	foreach(QVariant action, custom_actions)
		ajax::getSingletonPtr()->request(settings->url(), action);
}

void AjaxList::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();
	if( result.contains("error") )
		return;

	QVariantList rows = result.value("rows").toList();

	if(result.value("user_data").toString() == listUserData())
	{
		mItems = rows;
		mDataLoaded++;
	}
	else if(result.value("user_data").toString() == deleteUserData())
	{
		refresh();

		if(mAjaxView)
		{
			mAjaxView->cancel();
			mAjaxView->clear();
		}

		return;
	}
	else if(result.value("user_data").toString() == filterUserData())
	{
		QComboBox *combo = ui.filterCombo;

		int index = -1;
		bool blocked = combo->blockSignals(true);

		combo->clear();
		combo->addItem( tr("All") );

		for(int i = 0; i < rows.count(); i++)
		{
			QVariantMap map = rows.at(i).toMap();

			combo->addItem( itemText(map), itemID(map) );

			if(itemID(map) == mFilterID)
				index = i + 1;
		}

		if(index >= 0)
			combo->setCurrentIndex(index);
		else
			combo->setCurrentIndex(0);

		combo->blockSignals(blocked);

		mDataLoaded++;
	}
	else if(result.value("user_data").toString() == "auth_query_perms")
	{
		if( result.value("perms").toMap().value("modify_db").toBool() )
			ui.addButton->setVisible(true);
	}

	if(mDataLoaded)
	{
		updateFilter();
		updateSearch();
		setEnabled(true);
	}
}

int AjaxList::itemID(const QVariantMap& map) const
{
	return map["id"].toInt();
}

QString AjaxList::itemText(const QVariantMap& map) const
{
	return map[QString("name_%1").arg(settings->lang())].toString();
}

bool AjaxList::itemMatches(const QVariantMap& map, const QString& search)
{
	return (map["name_en"].toString().contains(search, Qt::CaseInsensitive) ||
		map["name_ja"].toString().contains(search, Qt::CaseInsensitive));
}
