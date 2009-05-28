/******************************************************************************\
*  client/src/BindMultiRelation.cpp                                            *
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

#include "BindMultiRelation.h"
#include "SearchEdit.h"
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QListWidget>

BindMultiRelation::BindMultiRelation(QObject *parent_object) :
	AjaxBind(parent_object), mViewer(0), mEditor(0), mAddList(0), mAddStack(0),
	mRemoveButton(0), mAddButton(0), mCancelButton(0), mNewButton(0),
	mQuickSearch(0)
{
	ajax::getSingletonPtr()->subscribe(this);
}

void BindMultiRelation::setColumn(const QString& col)
{
	QString c = col;
	c.replace("{$lang}", settings->lang());

	mColumns.clear();
	mColumns << c;
}

QStringList BindMultiRelation::columns() const
{
	// We don't want AjaxView to add in columns to it's view/update actions

	return QStringList();
}

QVariantList BindMultiRelation::customViewActions(int id) const
{
	Q_ASSERT(mColumns.count() == 1);

	QString col = mColumns.first();

	QVariantMap relation_columns;
	relation_columns[col] = QString("%1_%2").arg(
		mRelationForeignTable).arg(col);
	relation_columns["id"] = QString("%1_id").arg(mRelationForeignTable);

	if( !mExtraColumn.isEmpty() )
		relation_columns[mExtraColumn] = QString("%1_%2").arg(
			mRelationForeignTable).arg(mExtraColumn);

	QVariantMap relation;
	relation["id"] = mRelationForeignID;
	relation["table"] = mRelationForeignTable;
	relation["foreign_id"] = "id";
	relation["columns"] = relation_columns;

	QVariantMap action;
	action["action"] = "select";
	action["table"] = mOtherTable;
	action["relations"] = QVariantList() << relation;
	action["columns"] = QVariantList() << "id";

	QVariantMap where;
	where["column"] = QString("%1.%2").arg(mOtherTable).arg(mRelationID);
	where["type"] = "equals";
	where["value"] = id;

	action["where"] = QVariantList() << where;
	action["user_data"] = QString("multirel_%1_%2").arg(
		mTable).arg(mOtherTable);

	return QVariantList() << action << refreshItemCache();
}

QVariantList BindMultiRelation::customUpdateActions(int id) const
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

			action["rows"] = QVariantList() << row;

			actions << action;
		}
	}

	return actions;
}

QVariantList BindMultiRelation::customDeleteActions(int id) const
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

QWidget* BindMultiRelation::viewer() const
{
	return mViewer;
}

void BindMultiRelation::setViewer(QWidget *view)
{
	Q_ASSERT( view->inherits("QListWidget") );

	mViewer = qobject_cast<QListWidget*>(view);
}

QWidget* BindMultiRelation::editor() const
{
	return mEditor;
}

void BindMultiRelation::setEditor(QWidget *edit)
{
	Q_ASSERT( edit->inherits("QListWidget") );

	if(mEditor)
		disconnect(mEditor, SIGNAL(itemSelectionChanged()),
			this, SLOT(selectionChanged()));

	mEditor = qobject_cast<QListWidget*>(edit);

	connect(mEditor, SIGNAL(itemSelectionChanged()),
		this, SLOT(selectionChanged()));
}

void BindMultiRelation::clear()
{
	Q_ASSERT(mViewer && mEditor);

	mAddedIDs.clear();
	mRemoveQueue.clear();
	mInsertQueue.clear();

	mViewer->clear();
	mEditor->clear();

	if(mAddList)
		mAddList->clear();
	if(mAddStack)
		mAddStack->setCurrentIndex(0);
}

void BindMultiRelation::handleViewResponse(const QVariantMap& values)
{
	Q_UNUSED(values);

	// There is nothing from the main table we need
}

void BindMultiRelation::retrieveUpdateData(QVariantMap& row)
{
	Q_UNUSED(row);

	// There is nothing from the main table we need
}

QString BindMultiRelation::table() const
{
	return mTable;
}

void BindMultiRelation::setTable(const QString& tbl)
{
	mTable = tbl;
}

QString BindMultiRelation::otherTable() const
{
	return mOtherTable;
}

void BindMultiRelation::setOtherTable(const QString& tbl)
{
	mOtherTable = tbl;
}

QString BindMultiRelation::relationID() const
{
	return mRelationID;
}

QString BindMultiRelation::relationForeignTable() const
{
	return mRelationForeignTable;
}

QString BindMultiRelation::relationForeignID() const
{
	return mRelationForeignID;
}

void BindMultiRelation::setRelation(const QString& id,
	const QString& foreign_table, const QString& foreign_id)
{
	mRelationID = id;

	mRelationForeignTable = foreign_table;
	mRelationForeignID = foreign_id;
}

QString BindMultiRelation::extraColumn() const
{
	return mExtraColumn;
}

void BindMultiRelation::setExtraColumn(const QString& col)
{
	mExtraColumn = col;
	mExtraColumn.replace("{$lang}", settings->lang());
}

QPushButton* BindMultiRelation::removeButton()
{
	return mRemoveButton;
}

void BindMultiRelation::setRemoveButton(QPushButton *button)
{
	if(mRemoveButton)
		disconnect(mRemoveButton, SIGNAL(clicked(bool)),
			this, SLOT(queueRemoval()));

	mRemoveButton = button;

	connect(mRemoveButton, SIGNAL(clicked(bool)),
		this, SLOT(queueRemoval()));
}

QPushButton* BindMultiRelation::addButton()
{
	return mAddButton;
}

void BindMultiRelation::setAddButton(QPushButton *button)
{
	if(mAddButton)
		disconnect(mAddButton, SIGNAL(clicked(bool)),
			this, SLOT(startAdd()));

	mAddButton = button;

	connect(mAddButton, SIGNAL(clicked(bool)),
		this, SLOT(startAdd()));
}

QPushButton* BindMultiRelation::cancelButton()
{
	return mCancelButton;
}

void BindMultiRelation::setCancelButton(QPushButton *button)
{
	if(mCancelButton)
		disconnect(mCancelButton, SIGNAL(clicked(bool)),
			this, SLOT(cancelAdd()));

	mCancelButton = button;

	connect(mCancelButton, SIGNAL(clicked(bool)),
		this, SLOT(cancelAdd()));
}

QPushButton* BindMultiRelation::newButton()
{
	return mNewButton;
}

void BindMultiRelation::setNewButton(QPushButton *button)
{
	if(mNewButton)
		disconnect(mNewButton, SIGNAL(clicked(bool)),
			this, SLOT(performAdd()));

	mNewButton = button;

	connect(mNewButton, SIGNAL(clicked(bool)),
		this, SLOT(performAdd()));
}

QStackedWidget* BindMultiRelation::addStack()
{
	return mAddStack;
}

void BindMultiRelation::setAddStack(QStackedWidget *stack)
{
	mAddStack = stack;
}

QListWidget* BindMultiRelation::addList()
{
	return mAddList;
}

void BindMultiRelation::setAddList(QListWidget *list)
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

SearchEdit* BindMultiRelation::quickSearch()
{
	return mQuickSearch;
}

void BindMultiRelation::setQuickSearch(SearchEdit *edit)
{
	if(mQuickSearch)
		disconnect(mQuickSearch, SIGNAL(textChanged(const QString&)),
			this, SLOT(filterAddList()));

	mQuickSearch = edit;

	connect(mQuickSearch, SIGNAL(textChanged(const QString&)),
		this, SLOT(filterAddList()));
}

void BindMultiRelation::addRequested()
{
	ajax::getSingletonPtr()->request(refreshItemCache());
}

void BindMultiRelation::startAdd()
{
	Q_ASSERT(mEditor && mAddStack);

	mAddStack->setCurrentIndex(mAddStack->currentIndex() + 1);
}

void BindMultiRelation::cancelAdd()
{
	Q_ASSERT(mEditor && mAddStack);

	mAddStack->setCurrentIndex(mAddStack->currentIndex() - 1);
}

void BindMultiRelation::performAdd()
{
	Q_ASSERT(mEditor && mAddList);

	QList<QListWidgetItem*> items = mAddList->selectedItems();
	if( items.isEmpty() )
		return;

	Q_ASSERT(items.count() == 1);

	QListWidgetItem *item = items.first();
	Q_ASSERT(item);

	// Switch the view back
	cancelAdd();

	QVariantMap data = item->data(Qt::UserRole).toMap();

	QString extra_col = QString("%1_%2").arg(
		mRelationForeignTable).arg(mExtraColumn);

	data[mRelationForeignTable + "_name_en"] = data.value("name_en");
	data[mRelationForeignTable + "_name_ja"] = data.value("name_ja");
	if( !mExtraColumn.isEmpty() )
		data[extra_col] = data.value(mExtraColumn);
	data["__new__"] = true;

	QString name = QString("name_%1").arg(settings->lang());
	name = data.value(name).toString();

	if( !mExtraColumn.isEmpty() )
	{
		QStringList lines = data.value(extra_col).toString().split("\n");
		foreach(QString line, lines)
			name += QString("\n  %1").arg(line);
	}

	QListWidgetItem *new_item = new QListWidgetItem(name);
	new_item->setData(Qt::UserRole, data);

	mAddedIDs << data.value("id").toInt();
	mEditor->addItem(new_item);

	mInsertQueue << data;

	filterAddList();
}

void BindMultiRelation::queueRemoval()
{
	Q_ASSERT(mEditor);

	QList<QListWidgetItem*> items = mEditor->selectedItems();
	if( items.isEmpty() )
		return;

	Q_ASSERT(items.count() == 1);

	QListWidgetItem *item = items.first();
	Q_ASSERT(item);

	QMessageBox box(QMessageBox::Question, tr("Item Removal"), tr("Are you "
		"sure you wish to remove this item from the list?"), QMessageBox::Yes |
		QMessageBox::No);
	box.setDefaultButton(QMessageBox::Yes);

	if(box.exec() == QMessageBox::No)
		return;

	QVariantMap data = item->data(Qt::UserRole).toMap();

	if( !data.contains("__new__") )
	{
		mRemoveQueue << item->data(Qt::UserRole);

		mAddedIDs.removeAt( mAddedIDs.indexOf(
			data.value(mRelationForeignTable + "_id").toInt() ) );
	}
	else
	{
		// The item existed in the list, so make sure it's not in mInsertQueue

		int item_id = data.value("id").toInt();

		mAddedIDs.removeAt( mAddedIDs.indexOf(item_id) );

		for(int i = 0; i < mInsertQueue.count(); i++)
		{
			QVariantMap map = mInsertQueue.at(i).toMap();

			if(map.value("id").toInt() == item_id)
			{
				mInsertQueue.removeAt(i);
				break;
			}
		}
	}

	mEditor->removeItemWidget(item);
	delete item;

	filterAddList();
}

void BindMultiRelation::filterAddList()
{
	Q_ASSERT(mQuickSearch);

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

void BindMultiRelation::selectionChanged()
{
	Q_ASSERT(mEditor && mRemoveButton);

	bool selected = !mEditor->selectedItems().isEmpty();

	mRemoveButton->setEnabled(selected);
}

void BindMultiRelation::addSelectionChanged()
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

void BindMultiRelation::ajaxResponse(const QVariantMap& resp,
	const QString& user_data)
{
	Q_ASSERT(mColumns.count() == 1);
	Q_ASSERT(mViewer && mEditor);

	QString user_data2 = QString("multirel_%1_%2_cache").arg(
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

			if( !mExtraColumn.isEmpty() )
			{
				QStringList lines = row.value(
					mExtraColumn).toString().split("\n");

				foreach(QString line, lines)
					name += QString("\n  %1").arg(line);
			}

			QListWidgetItem *item = new QListWidgetItem(name);
			item->setData(Qt::UserRole, row);

			mAddList->addItem(item);
		}

		filterAddList();

		return;
	}

	user_data2 = QString("multirel_%1_%2").arg(mTable).arg(mOtherTable);

	if(resp.value("user_data").toString() != user_data2)
		return;

	QString name_col = QString("%1_%2").arg(
		mRelationForeignTable).arg( mColumns.first() );
	QString extra_col = QString("%1_%2").arg(
		mRelationForeignTable).arg(mExtraColumn);
	QString id_col = QString("%1_id").arg(mRelationForeignTable);

	QVariantList rows = resp.value("rows").toList();
	QListIterator<QVariant> it(rows);

	while( it.hasNext() )
	{
		QVariantMap row = it.next().toMap();

		QString name = row.value(name_col).toString();

		if( !mExtraColumn.isEmpty() )
		{
			QStringList lines = row.value(extra_col).toString().split("\n");
			foreach(QString line, lines)
				name += QString("\n  %1").arg(line);
		}

		QListWidgetItem *item = new QListWidgetItem(name);
		item->setData(Qt::UserRole, row);

		mAddedIDs << row.value(id_col).toInt();

		mViewer->addItem(item);
		mEditor->addItem( item->clone() );
	}
}

QVariant BindMultiRelation::refreshItemCache() const
{
	QVariantList cols;
	cols << QString("id") << QString("name_en") << QString("name_ja");
	if( !mExtraColumn.isEmpty() )
		cols << mExtraColumn;

	QVariantMap orderby_name;
	orderby_name["column"] = "name_en";
	orderby_name["direction"] = "asc";

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = mRelationForeignTable;
	action["columns"] = cols;
	action["order_by"] = QVariantList() << orderby_name;
	action["user_data"] = QString("multirel_%1_%2_cache").arg(
		mTable).arg(mOtherTable);

	return action;
}

bool BindMultiRelation::itemMatches(const QVariantMap& map,
	const QString& search)
{
	return (map["name_en"].toString().contains(search, Qt::CaseInsensitive) ||
		map["name_ja"].toString().contains(search, Qt::CaseInsensitive));
}
