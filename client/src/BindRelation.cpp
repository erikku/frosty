/******************************************************************************\
*  client/src/BindRelation.cpp                                                 *
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

#include "BindRelation.h"
#include "BasicRelationList.h"
#include "BasicRelationEdit.h"
#include "Settings.h"
#include "ajax.h"

#include <QtGui/QLabel>
#include <QtGui/QComboBox>

BindRelation::BindRelation(QObject *obj_parent) : AjaxBind(obj_parent),
	mViewer(0),	mEditor(0), mBrowseButton(0), mLastID(0), mFilled(false)
{
	// TODO: Allow the developer to set this
	mList = new BasicRelationList(new BasicRelationEdit);

	ajax::getSingletonPtr()->subscribe(this);
}

QWidget* BindRelation::viewer() const
{
	return mViewer;
}

void BindRelation::setViewer(QWidget *view)
{
	Q_ASSERT( view->inherits("QLabel") );

	mViewer = qobject_cast<QLabel*>(view);
}

QWidget* BindRelation::editor() const
{
	return mEditor;
}

void BindRelation::setEditor(QWidget *edit)
{
	Q_ASSERT( edit->inherits("QComboBox") );

	if(mEditor)
	{
		disconnect(mEditor, SIGNAL(currentIndexChanged(int)),
			this, SLOT(updateRelationCombo()));
	}

	mEditor = qobject_cast<QComboBox*>(edit);
	mEditor->clear();

	if( !mNoneName.isEmpty() )
		mEditor->addItem(mNoneName, 0);

	connect(mEditor, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateRelationCombo()));
}

void BindRelation::clear()
{
	Q_ASSERT( mViewer && mEditor );

	mViewer->clear();
	mEditor->setCurrentIndex(0);

	mLastID = -1;

	/*
	mEditor->clear();
	if( !mNoneName.isEmpty() )
		mEditor->addItem(mNoneName, 0);

	refreshRelationCache();
	*/
}

void BindRelation::handleViewResponse(const QVariantMap& values)
{
	Q_ASSERT( mViewer && mEditor );
	Q_ASSERT( values.contains(column()) );

	QVariant data = values.value(column());

	Q_ASSERT( data.canConvert<int>() );

	int id = data.toInt();

	mLastID = id;
	if(mFilled)
	{
		Q_ASSERT( mEditor->findData(id) != -1 );

		mViewer->setText( mCache.key(id) );
		mEditor->setCurrentIndex( mEditor->findData(id) );
		mEditor->setEnabled(true);
	}
}

void BindRelation::retrieveUpdateData(QVariantMap& row)
{
	Q_ASSERT( mViewer && mEditor );

	row[column()] = mEditor->itemData( mEditor->currentIndex() );
}

QString BindRelation::noneName() const
{
	return mNoneName;
}

void BindRelation::setNoneName(const QString& name)
{
	if(mEditor)
	{
		if(mEditor->itemData(0).toInt() == 0)
			mEditor->removeItem(0);

		mEditor->insertItem(0, name, 0);
	}

	mNoneName = name;
}

QString BindRelation::listTitle() const
{
	return mListTitle;
}

void BindRelation::setListTitle(const QString& title)
{
	mListTitle = title;
}

QVariantList BindRelation::filters() const
{
	return mFilters;
}

void BindRelation::setFilters(const QVariantList& f)
{
	mFilters = f;
}

QString BindRelation::foreignTable() const
{
	return mForeignTable;
}

void BindRelation::setForeignTable(const QString& table)
{
	mForeignTable = table;
}

QString BindRelation::foreignColumn() const
{
	return mForeignColumn;
}

void BindRelation::setForeignColumn(const QString& col)
{
	mForeignColumn = col;
}

QPushButton* BindRelation::browseButton() const
{
	return mBrowseButton;
}

void BindRelation::setBrowseButton(QPushButton *button)
{
	if(!button)
		return;

	if(mBrowseButton)
	{
		disconnect(mList, SIGNAL(relationsUpdated()),
			this, SLOT(refreshRelationCache()));
		disconnect(mBrowseButton, SIGNAL(clicked(bool)),
			this, SLOT(browseRelationList()));
	}

	mBrowseButton = button;

	connect(mList, SIGNAL(relationsUpdated()),
		this, SLOT(refreshRelationCache()));
	connect(mBrowseButton, SIGNAL(clicked(bool)),
		this, SLOT(browseRelationList()));
}

void BindRelation::browseRelationList()
{
	// TODO: Move this into the set functions
	// and swap this slot out for a simple
	// showList() w/o arguments
	mList->showList(mListTitle, mForeignTable);
}

void BindRelation::updateRelationCombo()
{
	if( !mEditor->isEnabled() )
		return;

	mLastID = mEditor->itemData( mEditor->currentIndex() ).toInt();
}

void BindRelation::refreshRelationCache()
{
	mCache = QVariantMap();
	mFilled = false;

	bool block = mEditor->blockSignals(true);
	mEditor->clear();
	mEditor->setEnabled(false);
	mEditor->blockSignals(block);

	QString column_name = mForeignColumn;
	if( column_name.isEmpty() )
		column_name = QString("name_%1").arg( settings->lang() );

	QVariantMap orderby_name;
	orderby_name["column"] = column_name;
	orderby_name["direction"] = "asc";

	QVariantMap orderby_id;
	orderby_id["column"] = "id";
	orderby_id["direction"] = "asc";

	QVariantList cols;
	cols << QString("id") << column_name;

	QVariantMap action;
	action["action"] = QString("select");
	action["table"] = mForeignTable;
	action["columns"] = cols;
	action["order_by"] = QVariantList() << orderby_name << orderby_id;
	action["user_data"] = QString("%1_%2").arg(mForeignTable).arg(
		"_relation_cache");

	if( !mFilters.isEmpty() )
		action["where"] = mFilters;

	ajax::getSingletonPtr()->request(settings->url(), action);
}

void BindRelation::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();

	if( mForeignTable.isEmpty() )
		return;

	Q_ASSERT( mViewer && mEditor );

	if( result.value("user_data").toString() != QString("%1_%2").arg(
		mForeignTable).arg("_relation_cache") )
			return;

	QString column_name = mForeignColumn;
	if( column_name.isEmpty() )
		column_name = QString("name_%1").arg( settings->lang() );

	// Clear the old cache
	mCache.clear();

	QVariantList rows = result.value("rows").toList();

	// Block signals until we are done
	bool block = mEditor->blockSignals(true);

	// Clear the editor
	mEditor->clear();
	if( !mNoneName.isEmpty() )
		mEditor->addItem(mNoneName, 0);

	int index = -1;

	for(int j = 0; j < rows.count(); j++)
	{
		QVariantMap map = rows.at(j).toMap();
		int id = map.value("id").toInt();

		mCache[ map.value(column_name).toString() ] = id;
		mEditor->addItem(map.value(column_name).toString(), id);

		if(id == mLastID)
			index = mEditor->count() - 1;
	}

	mEditor->blockSignals(block);
	mEditor->setEnabled(true);
	mFilled = true;

	if(index != -1)
	{
		mViewer->setText( mEditor->itemText(index) );
		mEditor->setCurrentIndex(index);
	}
}
