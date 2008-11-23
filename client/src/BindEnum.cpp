/******************************************************************************\
*  client/src/BindEnum.cpp                                                     *
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

#include "BindEnum.h"

#include <QtGui/QLabel>
#include <QtGui/QComboBox>

BindEnum::BindEnum(QObject *obj_parent) : AjaxBind(obj_parent),
	mViewer(0),	mEditor(0), mDefaultValue(0)
{
	// Nothing to see here
}

QWidget* BindEnum::viewer() const
{
	return mViewer;
}

void BindEnum::setViewer(QWidget *view)
{
	Q_ASSERT( view->inherits("QLabel") );

	mViewer = qobject_cast<QLabel*>(view);
}

QWidget* BindEnum::editor() const
{
	return mEditor;
}

void BindEnum::setEditor(QWidget *edit)
{
	Q_ASSERT( edit->inherits("QComboBox") );

	mEditor = qobject_cast<QComboBox*>(edit);
}

void BindEnum::clear()
{
	Q_ASSERT( mViewer && mEditor );

	mViewer->clear();

	Q_ASSERT( mEditor->findData(mDefaultValue) != -1 );

	mEditor->setCurrentIndex( mEditor->findData(mDefaultValue) );
}

void BindEnum::handleViewResponse(const QVariantMap& values)
{
	Q_ASSERT( mViewer && mEditor );
	Q_ASSERT( values.contains(column()) );

	QVariant data = values.value(column());

	Q_ASSERT( data.canConvert<int>() );

	int index = data.toInt();
	index = mEditor->findData(index);
	if(index == -1)
		index = mEditor->findData(mDefaultValue);

	Q_ASSERT(index != -1);

	mEditor->setCurrentIndex(index);
	mViewer->setText( mEditor->itemText(index) );
}

void BindEnum::retrieveUpdateData(QVariantMap& row)
{
	Q_ASSERT( mViewer && mEditor );

	row[column()] = mEditor->itemData( mEditor->currentIndex() );
}

int BindEnum::defaultValue() const
{
	return mDefaultValue;
}

void BindEnum::setDefaultValue(int value)
{
	mDefaultValue = value;
}

QMap<int, QString> BindEnum::items() const
{
	return mItems;
}

void BindEnum::setItems(const QMap<int, QString>& i)
{
	Q_ASSERT(i.count() >= 1);

	mItems = i;

	mEditor->clear();

	QMapIterator<int, QString> it(i);
	while( it.hasNext() )
	{
		it.next();

		Q_ASSERT( !it.value().isEmpty() );

		mEditor->addItem( it.value(), it.key() );
	}

	clear();
}
