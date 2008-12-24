/******************************************************************************\
*  client/src/IconListWidget.cpp                                               *
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

#include "IconListWidget.h"
#include "IconListWidgetItem.h"

#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>

IconListWidget::IconListWidget(QWidget *parent_widget) : QWidget(parent_widget)
{
	mLayout = new QVBoxLayout;
	mLayout->setContentsMargins(0, 0, 0, 0);
	mLayout->setSpacing(0);

	QVBoxLayout *masterLayout = new QVBoxLayout;
	masterLayout->setContentsMargins(0, 0, 0, 0);
	masterLayout->setSpacing(0);
	masterLayout->addLayout(mLayout);
	masterLayout->addStretch();

	QWidget *mainWidget = new QWidget;
	mainWidget->setLayout(masterLayout);

	mSelectionMode = IconListWidget::SingleSelection;

	mListArea = new QScrollArea;
	mListArea->setWidgetResizable(true);
	mListArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	mListArea->setWidget(mainWidget);

	QVBoxLayout *l = new QVBoxLayout;
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(mListArea);
	setLayout(l);
}

void IconListWidget::addItem(IconListWidgetItem *item)
{
	if( mItems.contains(item) )
		return;

	mLayout->addWidget(item);
	mItems << item;

	item->mIconListWidget = this;
	connect(item, SIGNAL(clicked()), this, SLOT(handleItemClicked()));
	connect(item, SIGNAL(doubleClicked()),
		this, SLOT(handleItemDoubleClicked()));
	connect(item, SIGNAL(selectionChanged()),
		this, SIGNAL(selectionChanged()));
}

void IconListWidget::addItem(const QIcon& icon, const QString& upper_text,
	const QString& lower_text)
{
	addItem(new IconListWidgetItem(icon, upper_text, lower_text));
}

void IconListWidget::insertItem(int r, IconListWidgetItem *item)
{
	if( mItems.contains(item) )
		return;

	mLayout->insertWidget(r, item);
	mItems.insert(r, item);

	item->mIconListWidget = this;

	connect(item, SIGNAL(clicked()), this, SLOT(handleItemClicked()));
	connect(item, SIGNAL(doubleClicked()),
		this, SLOT(handleItemDoubleClicked()));
	connect(item, SIGNAL(selectionChanged()),
		this, SIGNAL(selectionChanged()));
}

void IconListWidget::insertItem(int r, const QIcon& icon,
	const QString& upper_text, const QString& lower_text)
{
	insertItem(r, new IconListWidgetItem(icon, upper_text, lower_text));
}

IconListWidgetItem* IconListWidget::itemAt(int r) const
{
	if(mItems.count() <= r)
		return 0;

	return mItems.at(r);
}

int IconListWidget::row(IconListWidgetItem *item) const
{
	return mItems.indexOf( (IconListWidgetItem*)item );
}

void IconListWidget::removeItem(int r)
{
	delete takeItem(r);
}

void IconListWidget::removeItem(IconListWidgetItem *item)
{
	delete takeItem(item);
}

IconListWidgetItem* IconListWidget::takeItem(int r)
{
	if(mItems.count() >= r)
		return 0;

	IconListWidgetItem *item = mItems.takeAt(r);
	mLayout->removeWidget(item);

	item->mIconListWidget = 0;

	disconnect(item, SIGNAL(clicked()), this, SLOT(handleItemClicked()));
	disconnect(item, SIGNAL(doubleClicked()),
		this, SLOT(handleItemDoubleClicked()));
	disconnect(item, SIGNAL(selectionChanged()),
		this, SIGNAL(selectionChanged()));

	if( item->isSelected() )
		emit selectionChanged();

	return item;
}

IconListWidgetItem* IconListWidget::takeItem(IconListWidgetItem *item)
{
	if( !mItems.contains(item) )
		return 0;

	mItems.removeAt( mItems.indexOf(item) );
	mLayout->removeWidget(item);

	item->mIconListWidget = 0;

	disconnect(item, SIGNAL(clicked()), this, SLOT(handleItemClicked()));
	disconnect(item, SIGNAL(doubleClicked()),
		this, SLOT(handleItemDoubleClicked()));
	disconnect(item, SIGNAL(selectionChanged()),
		this, SIGNAL(selectionChanged()));

	if( item->isSelected() )
		emit selectionChanged();

	return item;
}

int IconListWidget::count() const
{
	return mItems.count();
}

QList<IconListWidgetItem*> IconListWidget::selectedItems() const
{
	QList<IconListWidgetItem*> selected;

	foreach(IconListWidgetItem *item, mItems)
	{
		if( item->isSelected() )
			selected << item;
	}

	return selected;
}

void IconListWidget::setCurrentRow(int r)
{
	if(mItems.count() >= r)
		return;

	foreach(IconListWidgetItem *item, mItems)
		item->setSelected(false);

	mItems.at(r)->setSelected(true);
}

void IconListWidget::setCurrentItem(IconListWidgetItem *item)
{
	if( !mItems.contains(item) )
		return;

	foreach(IconListWidgetItem *i, mItems)
		i->setSelected(false);

	item->setSelected(true);
}

void IconListWidget::clear()
{
	foreach(IconListWidgetItem *item, mItems)
	{
		mLayout->removeWidget(item);
		delete item;
	}

	mItems.clear();

	emit selectionChanged();
}

void IconListWidget::handleItemClicked()
{
	IconListWidgetItem *item = qobject_cast<IconListWidgetItem*>( sender() );
	if(!item)
		return;

	switch(mSelectionMode)
	{
		case IconListWidget::SingleSelection:
		{
			bool blocked = item->blockSignals(true);
			foreach(IconListWidgetItem *i, mItems)
				i->setSelected(false);

			item->blockSignals(blocked);
			item->setSelected(true);
			break;
		}
		case IconListWidget::MultiSelection:
		{
			item->setSelected( !item->isSelected() );
			break;
		}
		default:
			break;
	}

	emit itemClicked(item);
}

void IconListWidget::handleItemDoubleClicked()
{
	IconListWidgetItem *item = qobject_cast<IconListWidgetItem*>( sender() );
	if(!item)
		return;

	emit itemDoubleClicked(item);
}

IconListWidget::SelectionMode IconListWidget::selectionMode() const
{
	return mSelectionMode;
}

void IconListWidget::setSelectionMode(SelectionMode mode)
{
	if(mode == SingleSelection)
	{
		QList<IconListWidgetItem*> items = selectedItems();
		if(items.count() > 1)
		{
			items.removeFirst();
			foreach(IconListWidgetItem *item, items)
				item->setSelected(false);
		}
	}

	mSelectionMode = mode;
}
