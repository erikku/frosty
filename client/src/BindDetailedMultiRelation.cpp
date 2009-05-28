/******************************************************************************\
*  client/src/BindDetailedMultiRelation.cpp                                    *
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

#include "BindDetailedMultiRelation.h"
#include "InfoListWidgetItem.h"
#include "InfoListWidget.h"
#include "SearchEdit.h"
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QInputDialog>
#include <QtGui/QStackedWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QListWidget>

void BindDetailedMultiRelation::multiRelationColumn(const QString& col_arg)
{
	QString col = col_arg;
	col.replace("{$lang}", settings->lang());

	// If it's an empty column, just add it to the list and return
	if( col.isEmpty() )
	{
		mColumns << QString();
		return;
	}

	// If the column is from a custom relation, we should leave it alone
	// If the column is __extra__ we need to replace it with the proper column
	// If the column is from the related table, we should prefix it

	QString alias;
	if( mForeignColumns.contains(col) )
	{
		alias = col;
	}
	else if(col == "__extra__")
	{
		Q_ASSERT( !mExtraColumn.isEmpty() );

		alias = mExtraColumn;
	}
	else
	{
		alias = QString("%1_%2").arg(mRelationForeignTable).arg(col);

		// Since the column is from the related table, add it to the column
		// list for the relation
		mRelationColumns[col] = alias;
	}

	mColumns << alias;
}

BindDetailedMultiRelation::BindDetailedMultiRelation(QObject *obj_parent) :
	AjaxBind(obj_parent), mViewer(0), mEditor(0), mAddList(0), mAddStack(0),
	mRemoveButton(0), mEditButton(0), mAddButton(0), mCancelButton(0),
	mNewButton(0), mQuickSearch(0), mCurrentID(0)
{
	ajax::getSingletonPtr()->subscribe(this);
}

void BindDetailedMultiRelation::setColumns(const QStringList& cols)
{
	Q_ASSERT(cols.count() == 4);

	mColumns.clear();
	mRelationColumns.clear();

	multiRelationColumn(mIconColumn);
	mIconColumn = mColumns.takeFirst();

	multiRelationColumn(cols.at(0));
	multiRelationColumn(cols.at(1));
	multiRelationColumn(cols.at(2));
	multiRelationColumn(cols.at(3));
}

QStringList BindDetailedMultiRelation::columns() const
{
	// We don't want AjaxView to add in columns to it's view/update actions

	return QStringList();
}

QVariantList BindDetailedMultiRelation::customViewActions(int id) const
{
	// Add in the ID
	QVariantMap relation_columns = mRelationColumns;
	relation_columns["id"] = QString("%1_id").arg(mRelationForeignTable);

	QVariantMap relation;
	relation["id"] = mRelationForeignID;
	relation["table"] = mRelationForeignTable;
	relation["foreign_id"] = "id";
	relation["columns"] = relation_columns;

	QVariantMap action;
	action["action"] = "select";
	action["table"] = mOtherTable;

	if( mExtraColumn.isEmpty() )
		action["columns"] = QVariantList() << "id";
	else
		action["columns"] = QVariantList() << "id" << mExtraColumn;

	if( mAdditionalRelations.isEmpty() )
		action["relations"] = QVariantList() << relation;
	else
		action["relations"] = QVariantList() << relation
			<< mAdditionalRelations;

	mCurrentID = id;

	QVariantMap where;
	where["column"] = QString("%1.%2").arg(mOtherTable).arg(mRelationID);
	where["type"] = "equals";
	where["value"] = id;

	action["where"] = QVariantList() << where;
	action["user_data"] = QString("multi_relation_%1_%2_%3").arg(
		mTable).arg(mOtherTable).arg(mCurrentID);

	return QVariantList() << action << refreshItemCache();
}

QVariantList BindDetailedMultiRelation::customUpdateActions(int id) const
{
	QVariantList actions;

	// Process remove queue
	if(id > 0)
	{
		foreach(QVariant i, mRemoveQueue)
		{
			QVariantMap action;
			action["action"] = "delete";
			action["table"] = mOtherTable;

			QVariantMap where;
			where["column"] = "id";
			where["type"] = "equals";
			where["value"] = i.toMap().value("id");

			action["where"] = QVariantList() << where;

			actions << action;
		}
	}

	// Process extra data to update
	if( !mExtraColumn.isEmpty() )
	{
		foreach(QVariant i, mUpdateQueue)
		{
			QVariantMap map = i.toMap();

			QVariantMap action;
			action["action"] = "update";
			action["table"] = mOtherTable;

			QVariantMap row;
			row[mExtraColumn] = map.value(mExtraColumn);

			QVariantMap where;
			where["column"] = "id";
			where["type"] = "equals";
			where["value"] = map.value("id");

			action["rows"] = QVariantList() << row;
			action["where"] = QVariantList() << where;

			actions << action;
		}
	}

	// TODO: This will not work if you are inserting a new record since
	// we don't know the id for it yet

	// Process insert queue
	{
		foreach(QVariant i, mInsertQueue)
		{
			QVariantMap map = i.toMap();

			QVariantMap action;
			action["action"] = "insert";
			action["table"] = mOtherTable;

			QVariantMap row;
			row[mRelationID] = id;
			row[mRelationForeignID] = map.value("id");
			row[mExtraColumn] = map.value(mExtraColumn);

			action["rows"] = QVariantList() << row;

			actions << action;
		}
	}

	return actions;
}

QVariantList BindDetailedMultiRelation::customDeleteActions(int id) const
{
	QVariantMap action;
	action["action"] = "delete";
	action["table"] = mOtherTable;

	QVariantMap where;
	where["column"] = mRelationID;
	where["type"] = "equals";
	where["value"] = id;

	action["where"] = QVariantList() << where;

	return QVariantList() << action;
}

QWidget* BindDetailedMultiRelation::viewer() const
{
	return mViewer;
}

void BindDetailedMultiRelation::setViewer(QWidget *view)
{
	Q_ASSERT( view->inherits("InfoListWidget") );

	mViewer = qobject_cast<InfoListWidget*>(view);
	mViewer->setSelectionMode(InfoListWidget::NoSelection);
}

QWidget* BindDetailedMultiRelation::editor() const
{
	return mEditor;
}

void BindDetailedMultiRelation::setEditor(QWidget *edit)
{
	Q_ASSERT( edit->inherits("InfoListWidget") );

	if(mEditor)
		disconnect(mEditor, SIGNAL(selectionChanged()),
			this, SLOT(selectionChanged()));

	mEditor = qobject_cast<InfoListWidget*>(edit);

	connect(mEditor, SIGNAL(selectionChanged()),
		this, SLOT(selectionChanged()));
}

void BindDetailedMultiRelation::clear()
{
	Q_ASSERT(mViewer);

	mViewer->clear();

	mAddedIDs.clear();
	mRemoveQueue.clear();
	mUpdateQueue.clear();
	mInsertQueue.clear();

	if(mEditor)
		mEditor->clear();
	if(mAddList)
		mAddList->clear();
	if(mAddStack)
		mAddStack->setCurrentIndex(0);
}

void BindDetailedMultiRelation::handleViewResponse(const QVariantMap& values)
{
	Q_UNUSED(values);

	// There is nothing from the main table we need
}

void BindDetailedMultiRelation::retrieveUpdateData(QVariantMap& row)
{
	Q_UNUSED(row);

	// There is nothing from the main table we need
}

QString BindDetailedMultiRelation::relationID() const
{
	return mRelationID;
}

QString BindDetailedMultiRelation::relationForeignTable() const
{
	return mRelationForeignTable;
}

QString BindDetailedMultiRelation::relationForeignID() const
{
	return mRelationForeignID;
}

void BindDetailedMultiRelation::setRelation(const QString& id,
	const QString& foreign_table, const QString& foreign_id)
{
	mRelationID = id;

	mRelationForeignTable = foreign_table;
	mRelationForeignID = foreign_id;
}

QString BindDetailedMultiRelation::iconPath() const
{
	return mIconPath;
}

void BindDetailedMultiRelation::setIconPath(const QString& path)
{
	mIconPath = path;
}

QString BindDetailedMultiRelation::iconColumn() const
{
	return  mIconColumn;
}

void BindDetailedMultiRelation::setIconColumn(const QString& col)
{
	mIconColumn = col;
}

QString BindDetailedMultiRelation::extraColumn() const
{
	return mExtraColumn;
}

void BindDetailedMultiRelation::setExtraColumn(const QString& col)
{
	mExtraColumn = col;
}

QString BindDetailedMultiRelation::extraPrompt() const
{
	return mExtraPrompt;
}

void BindDetailedMultiRelation::setExtraPrompt(const QString& prompt)
{
	mExtraPrompt = prompt;
}

QString BindDetailedMultiRelation::table() const
{
	return mTable;
}

void BindDetailedMultiRelation::setTable(const QString& tbl)
{
	mTable = tbl;
}

QString BindDetailedMultiRelation::otherTable() const
{
	return mOtherTable;
}

void BindDetailedMultiRelation::setOtherTable(const QString& tbl)
{
	mOtherTable = tbl;
}

QVariantList BindDetailedMultiRelation::additionalRelations() const
{
	return mAdditionalRelations;
}

void BindDetailedMultiRelation::setAdditionalRelations(
	const QVariantList& additional_relations)
{
	mAdditionalRelations = additional_relations;

	mForeignColumns.clear();

	QListIterator<QVariant> it(mAdditionalRelations);
	while( it.hasNext() )
	{
		QVariantMap relation = it.next().toMap();
		QVariantMap cols = relation.value("columns").toMap();

		QMapIterator<QString, QVariant> col_it(cols);
		while( col_it.hasNext() )
		{
			col_it.next();

			mForeignColumns << col_it.value().toString();
		}
	}
}

void BindDetailedMultiRelation::ajaxResponse(const QVariantMap& resp,
	const QString& user_data)
{
	QString user_data2 = QString("multi_relation_%1_%2_cache").arg(
		mTable).arg(mOtherTable);

	if(user_data == user_data2)
	{
		Q_ASSERT(mAddList);

		QVariantList rows = resp.value("rows").toList();
		QListIterator<QVariant> it(rows);

		mAddList->clear();

		while( it.hasNext() )
		{
			QVariantMap row = it.next().toMap();

			QString name = QString("name_%1").arg(settings->lang());
			name = row.value(name).toString();

			QString icon_path = mIconPath.arg( row.value("icon").toString() );
			icon_path = icon_path.replace("icon_", "icon16_");

			QIcon icon = QIcon(icon_path);

			QListWidgetItem *item = new QListWidgetItem(icon, name);
			item->setData(Qt::UserRole, row);

			mAddList->addItem(item);
		}

		filterAddList();

		return;
	}

	user_data2 = QString("multi_relation_%1_%2_%3").arg(
		mTable).arg(mOtherTable).arg(mCurrentID);

	if(user_data != user_data2)
		return;

	QVariantList rows = resp.value("rows").toList();
	QListIterator<QVariant> it(rows);

	while( it.hasNext() )
	{
		QVariantMap row = it.next().toMap();

		QStringList text;
		text << QString() << QString() << QString() << QString();

		// Process the icon
		QIcon icon;
		if( !mIconColumn.isEmpty() )
			icon = QIcon( mIconPath.arg( row.value(mIconColumn).toString() ) );

		for(int i = 0; i < 4; i++)
		{
			QVariant item;

			if( !mColumns.at(i).isEmpty() )
				item = row.value( mColumns.at(i) );

			if(item.canConvert(QVariant::Int) && item.toInt() == -1)
				text[i] = tr("-");
			else
				text[i] = item.toString();
		}

		InfoListWidgetItem *item = new InfoListWidgetItem(icon, text[0],
			text[1], text[2], text[3]);
		item->setData(row);

		mAddedIDs << row.value(mRelationForeignTable + "_id").toInt();

		mViewer->addItem(item);

		if(mEditor)
			mEditor->addItem( item->clone() );
	}
}

void BindDetailedMultiRelation::selectionChanged()
{
	if(!mEditor)
		return;

	bool selected = !mEditor->selectedItems().isEmpty();

	if(mRemoveButton)
		mRemoveButton->setEnabled(selected);
	if(mEditButton)
		mEditButton->setEnabled(selected);
}

void BindDetailedMultiRelation::addSelectionChanged()
{
	if(!mAddList)
		return;

	QList<QListWidgetItem*> items = mAddList->selectedItems();

	if( items.isEmpty() )
		mNewButton->setEnabled(false);
	else if( items.first()->isHidden() )
		mNewButton->setEnabled(false);
	else
		mNewButton->setEnabled(true);
}

QPushButton* BindDetailedMultiRelation::removeButton()
{
	return mRemoveButton;
}

void BindDetailedMultiRelation::setRemoveButton(QPushButton *button)
{
	if(mRemoveButton)
		disconnect(mRemoveButton, SIGNAL(clicked(bool)),
			this, SLOT(queueRemoval()));

	mRemoveButton = button;

	connect(mRemoveButton, SIGNAL(clicked(bool)),
		this, SLOT(queueRemoval()));
}

QPushButton* BindDetailedMultiRelation::editButton()
{
	return mEditButton;
}

void BindDetailedMultiRelation::setEditButton(QPushButton *button)
{
	if(mEditButton)
		disconnect(mEditButton, SIGNAL(clicked(bool)),
			this, SLOT(performEdit()));

	mEditButton = button;

	connect(mEditButton, SIGNAL(clicked(bool)),
		this, SLOT(performEdit()));
}

QPushButton* BindDetailedMultiRelation::addButton()
{
	return mAddButton;
}

void BindDetailedMultiRelation::setAddButton(QPushButton *button)
{
	if(mAddButton)
		disconnect(mAddButton, SIGNAL(clicked(bool)),
			this, SLOT(startAdd()));

	mAddButton = button;

	connect(mAddButton, SIGNAL(clicked(bool)),
		this, SLOT(startAdd()));
}

QPushButton* BindDetailedMultiRelation::cancelButton()
{
	return mCancelButton;
}

void BindDetailedMultiRelation::setCancelButton(QPushButton *button)
{
	if(mCancelButton)
		disconnect(mCancelButton, SIGNAL(clicked(bool)),
			this, SLOT(cancelAdd()));

	mCancelButton = button;

	connect(mCancelButton, SIGNAL(clicked(bool)),
		this, SLOT(cancelAdd()));
}

QPushButton* BindDetailedMultiRelation::newButton()
{
	return mNewButton;
}

void BindDetailedMultiRelation::setNewButton(QPushButton *button)
{
	if(mNewButton)
		disconnect(mNewButton, SIGNAL(clicked(bool)),
			this, SLOT(performAdd()));

	mNewButton = button;

	connect(mNewButton, SIGNAL(clicked(bool)),
		this, SLOT(performAdd()));
}

QStackedWidget* BindDetailedMultiRelation::addStack()
{
	return mAddStack;
}

void BindDetailedMultiRelation::setAddStack(QStackedWidget *stack)
{
	mAddStack = stack;
}

QListWidget* BindDetailedMultiRelation::addList()
{
	return mAddList;
}

void BindDetailedMultiRelation::setAddList(QListWidget *list)
{
	if(mAddList)
	{
		disconnect(mAddList, SIGNAL(itemSelectionChanged()),
			this, SLOT(addSelectionChanged()));
		disconnect(mAddList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
			this, SLOT(performAdd()));
	}

	mAddList = list;

	addSelectionChanged();

	connect(mAddList, SIGNAL(itemSelectionChanged()),
		this, SLOT(addSelectionChanged()));
	connect(mAddList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
		this, SLOT(performAdd()));
}

SearchEdit* BindDetailedMultiRelation::quickSearch()
{
	return mQuickSearch;
}

void BindDetailedMultiRelation::setQuickSearch(SearchEdit *edit)
{
	if(mQuickSearch)
		disconnect(mQuickSearch, SIGNAL(textChanged(const QString&)),
			this, SLOT(filterAddList()));

	mQuickSearch = edit;

	connect(mQuickSearch, SIGNAL(textChanged(const QString&)),
		this, SLOT(filterAddList()));
}

void BindDetailedMultiRelation::queueRemoval()
{
	if(!mEditor)
		return;

	QList<InfoListWidgetItem*> items = mEditor->selectedItems();
	if( items.isEmpty() )
		return;

	Q_ASSERT(items.count() == 1);

	InfoListWidgetItem *item = items.first();
	Q_ASSERT(item);

	QMessageBox box(QMessageBox::Question, tr("Item Removal"), tr("Are you "
		"sure you wish to remove this item from the list?"), QMessageBox::Yes |
		QMessageBox::No);
	box.setDefaultButton(QMessageBox::Yes);

	if(box.exec() == QMessageBox::No)
		return;

	QVariantMap data = item->data().toMap();

	if( !data.contains("__new__") )
	{
		mRemoveQueue << item->data();

		mAddedIDs.removeAt( mAddedIDs.indexOf(
			data.value(mRelationForeignTable + "_id").toInt() ) );
	}
	else
	{
		// The item existed in the list, so make sure it's not in
		// mUpdateQueue or mInsertQueue

		int skill_id = data.value("id").toInt();

		mAddedIDs.removeAt( mAddedIDs.indexOf(skill_id) );

		for(int i = 0; i < mUpdateQueue.count(); i++)
		{
			QVariantMap map = mUpdateQueue.at(i).toMap();

			if(map.value("id").toInt() == skill_id)
			{
				mUpdateQueue.removeAt(i);
				break;
			}
		}

		for(int i = 0; i < mInsertQueue.count(); i++)
		{
			QVariantMap map = mInsertQueue.at(i).toMap();

			if(map.value("id").toInt() == skill_id)
			{
				mInsertQueue.removeAt(i);
				break;
			}
		}
	}

	mEditor->removeItem(item);

	filterAddList();
}

void BindDetailedMultiRelation::performEdit()
{
	if(!mEditor)
		return;

	QList<InfoListWidgetItem*> items = mEditor->selectedItems();
	if( items.isEmpty() )
		return;

	Q_ASSERT(items.count() == 1);

	InfoListWidgetItem *item = items.first();
	Q_ASSERT(item);

	QVariantMap data = item->data().toMap();

	int value = promptForExtra( data.value(mExtraColumn).toInt() );

	data[mExtraColumn] = value;
	item->setData(data);

	for(int i = 0; i < 4; i++)
	{
		if(mColumns.at(i) != mExtraColumn)
			continue;

		if(value == -1)
			item->setTextAt(i, tr("-"));
		else
			item->setTextAt(i, QString::number(value));

		break;
	}

	// If the item is in the mInsertQueue it should be edited there
	// instead of added to the mUpdateQueue (since it's a waste to make 2
	// database calls and if the order of the 2 calls are wrong, it will
	// cause an error)
	if( data.contains("__new__") )
	{
		int skill_id = data.value("id").toInt();

		for(int i = 0; i < mInsertQueue.count(); i++)
		{
			QVariantMap insert_item = mInsertQueue.at(i).toMap();

			if(insert_item.value("id").toInt() != skill_id)
				continue;

			mInsertQueue[i] = data;
			break;
		}

		return;
	}

	// If the item has already been edited, remove it from the update queue
	for(int i = 0; i < mUpdateQueue.count(); i++)
	{
		QVariantMap queue_item = mUpdateQueue.at(i).toMap();

		if( queue_item.value("id").toInt() == data.value("id").toInt() )
		{
			mUpdateQueue.removeAt(i);

			break;
		}
	}

	mUpdateQueue << data;
}

void BindDetailedMultiRelation::startAdd()
{
	if(!mEditor || !mAddStack)
		return;

	mAddStack->setCurrentIndex(mAddStack->currentIndex() + 1);
}

void BindDetailedMultiRelation::cancelAdd()
{
	if(!mEditor || !mAddStack)
		return;

	mAddStack->setCurrentIndex(mAddStack->currentIndex() - 1);
}

void BindDetailedMultiRelation::performAdd()
{
	Q_ASSERT(mEditor);

	QList<QListWidgetItem*> items = mAddList->selectedItems();
	if( items.isEmpty() )
		return;

	Q_ASSERT(items.count() == 1);

	QListWidgetItem *item = items.first();
	Q_ASSERT(item);

	// Switch the view back
	cancelAdd();

	QVariantMap data = item->data(Qt::UserRole).toMap();

	// Ask for the extra data
	if( !mExtraColumn.isEmpty() )
	{
		int extra = promptForExtra(-2);
		if(extra == -2)
			return;

		data[mExtraColumn] = extra;
	}

	data[mRelationForeignTable + "_icon"] = data.value("icon");
	data[mRelationForeignTable + "_name_en"] = data.value("name_en");
	data[mRelationForeignTable + "_name_ja"] = data.value("name_ja");
	data["__new__"] = true;

	// Add the item to the list
	QStringList text;
	text << QString() << QString() << QString() << QString();

	// Process the icon
	QIcon icon;
	if( !mIconColumn.isEmpty() )
		icon = QIcon( mIconPath.arg( data.value(mIconColumn).toString() ) );

	for(int i = 0; i < 4; i++)
	{
		QVariant j;

		if( !mColumns.at(i).isEmpty() )
			j = data.value( mColumns.at(i) );

		if(j.canConvert(QVariant::Int) && j.toInt() == -1)
			text[i] = tr("-");
		else
			text[i] = j.toString();
	}

	InfoListWidgetItem *new_item = new InfoListWidgetItem(icon, text[0],
		text[1], text[2], text[3]);
	new_item->setData(data);

	mAddedIDs << data.value("id").toInt();
	mEditor->addItem(new_item);

	mInsertQueue << data;

	filterAddList();
}

int BindDetailedMultiRelation::promptForExtra(int default_value)
{
	if(!mEditor)
		return default_value;

	bool ok;
	int extra = QInputDialog::getInteger(0, tr("Data Entry"), mExtraPrompt,
		default_value == -2 ? -1 : default_value, -1, 99, 1, &ok);

	return ok ? extra : default_value;
}

QVariant BindDetailedMultiRelation::refreshItemCache() const
{
	QVariantList cols;
	cols << QString("id") << QString("name_en") << QString("name_ja")
		<< QString("icon");

	// TODO: Allow the developer to set this query sort order (or most
	// of the query)

	QVariantMap orderby_name;
	orderby_name["column"] = "name_en";
	orderby_name["direction"] = "asc";

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = QString("db_skills");
	action["columns"] = cols;
	action["order_by"] = QVariantList() << orderby_name;
	action["relations"] = QVariantList() << mAdditionalRelations;
	action["user_data"] = QString("multi_relation_%1_%2_cache").arg(
		mTable).arg(mOtherTable);

	return action;
}

void BindDetailedMultiRelation::filterAddList()
{
	QString search = mQuickSearch->text();
	QListWidget *list = mAddList;

	if( search.isEmpty() )
	{
		// Show all items
		for(int i = 0; i < list->count(); i++)
		{
			QVariantMap map = list->item(i)->data(Qt::UserRole).toMap();
			int id = map.value("id").toInt();

			list->item(i)->setHidden( mAddedIDs.contains(id) );
		}

		return;
	}

	for(int i = 0; i < list->count(); i++)
	{
		QListWidgetItem *item = list->item(i);
		QVariantMap map = item->data(Qt::UserRole).toMap();
		int id = map.value("id").toInt();

		item->setHidden( !itemMatches(map, search) || mAddedIDs.contains(id) );
	}

	addSelectionChanged();
}

bool BindDetailedMultiRelation::itemMatches(const QVariantMap& map,
	const QString& search)
{
	return (map["name_en"].toString().contains(search, Qt::CaseInsensitive) ||
		map["name_ja"].toString().contains(search, Qt::CaseInsensitive));
}

void BindDetailedMultiRelation::addRequested()
{
	ajax::getSingletonPtr()->request(refreshItemCache());
}
