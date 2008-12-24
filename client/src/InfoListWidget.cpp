/******************************************************************************\
*  client/src/InfoListWidget.cpp                                               *
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

#include "InfoListWidget.h"
#include "InfoListWidgetItem.h"

#include <QtGui/QVBoxLayout>

bool defaultSortFunction1(const InfoListWidgetItem *item1,
	const InfoListWidgetItem *item2)
{
	return item1->text1() < item2->text1();
}

bool defaultSortFunction2(const InfoListWidgetItem *item1,
	const InfoListWidgetItem *item2)
{
	return item1->text2() < item2->text2();
}

bool defaultSortFunction3(const InfoListWidgetItem *item1,
	const InfoListWidgetItem *item2)
{
	return item1->text3() < item2->text3();
}

bool defaultSortFunction4(const InfoListWidgetItem *item1,
	const InfoListWidgetItem *item2)
{
	return item1->text4() < item2->text4();
}

InfoListWidget::InfoListWidget(QWidget *parent_widget) : QWidget(parent_widget)
{
	ui.setupUi(this);

	mSortingEnabled = true;
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

	setSortHeaders("Name", "Level", "Genus", "LNC");

	mIconUp1 = QPixmap(":/up1.png");
	mIconUp2 = QPixmap(":/up2.png");
	mIconDown1 = QPixmap(":/down1.png");
	mIconDown2 = QPixmap(":/down2.png");

	ui.sortArrow1->setPixmap(mIconUp2);
	ui.sortArrow2->setPixmap(mIconUp1);
	ui.sortArrow3->setPixmap(mIconUp1);
	ui.sortArrow4->setPixmap(mIconUp1);

	mSelectionMode = InfoListWidget::SingleSelection;

	mSortOrder << 0 << 1 << 2 << 3;
	mSortDirection << 1 << 1 << 1 << 1;
	mSortFunctions << defaultSortFunction1 << defaultSortFunction2
		<< defaultSortFunction3 << defaultSortFunction4;

	ui.listArea->setWidget(mainWidget);

	connect(ui.sort1, SIGNAL(clicked()), this, SLOT(toggleSort1()));
	connect(ui.sort2, SIGNAL(clicked()), this, SLOT(toggleSort2()));
	connect(ui.sort3, SIGNAL(clicked()), this, SLOT(toggleSort3()));
	connect(ui.sort4, SIGNAL(clicked()), this, SLOT(toggleSort4()));
	connect(ui.sortArrow1, SIGNAL(clicked()), this, SLOT(toggleSort1()));
	connect(ui.sortArrow2, SIGNAL(clicked()), this, SLOT(toggleSort2()));
	connect(ui.sortArrow3, SIGNAL(clicked()), this, SLOT(toggleSort3()));
	connect(ui.sortArrow4, SIGNAL(clicked()), this, SLOT(toggleSort4()));
}

void InfoListWidget::addItem(InfoListWidgetItem *item)
{
	if( mItems.contains(item) )
		return;

	if(mSortingEnabled)
	{
		int i = 0;
		for(i = mItems.count() - 1; i >= 0 && lessThen(item, mItems[i]); i--);

		mLayout->insertWidget(++i, item);
		mItems.insert(i, item);
	}
	else
	{
		mLayout->addWidget(item);
		mItems << item;
	}

	item->mInfoListWidget = this;

	connect(item, SIGNAL(clicked()), this, SLOT(handleItemClicked()));
	connect(item, SIGNAL(doubleClicked()),
		this, SLOT(handleItemDoubleClicked()));
	connect(item, SIGNAL(selectionChanged()),
		this, SIGNAL(selectionChanged()));
}

void InfoListWidget::addItem(const QIcon& icon, const QString& text1,
	const QString& text2, const QString& text3, const QString& text4)
{
	addItem(new InfoListWidgetItem(icon, text1, text2, text3, text4));
}

void InfoListWidget::insertItem(int r, InfoListWidgetItem *item)
{
	if( mItems.contains(item) )
		return;

	mLayout->insertWidget(r, item);
	mItems.insert(r, item);

	item->mInfoListWidget = this;

	connect(item, SIGNAL(clicked()), this, SLOT(handleItemClicked()));
	connect(item, SIGNAL(doubleClicked()),
		this, SLOT(handleItemDoubleClicked()));
	connect(item, SIGNAL(selectionChanged()),
		this, SIGNAL(selectionChanged()));
}

void InfoListWidget::insertItem(int r, const QIcon& icon,
	const QString& text1, const QString& text2, const QString& text3,
	const QString& text4)
{
	insertItem(r, new InfoListWidgetItem(icon, text1, text2, text3, text4));
}

InfoListWidgetItem* InfoListWidget::itemAt(int r) const
{
	if(mItems.count() <= r)
		return 0;

	return mItems.at(r);
}

int InfoListWidget::row(InfoListWidgetItem *item) const
{
	return mItems.indexOf( (InfoListWidgetItem*)item );
}

void InfoListWidget::removeItem(int r)
{
	delete takeItem(r);
}

void InfoListWidget::removeItem(InfoListWidgetItem *item)
{
	delete takeItem(item);
}

InfoListWidgetItem* InfoListWidget::takeItem(int r)
{
	if(mItems.count() >= r)
		return 0;

	InfoListWidgetItem *item = mItems.takeAt(r);
	mLayout->removeWidget(item);

	item->mInfoListWidget = 0;

	disconnect(item, SIGNAL(clicked()), this, SLOT(handleItemClicked()));
	disconnect(item, SIGNAL(doubleClicked()),
		this, SLOT(handleItemDoubleClicked()));
	disconnect(item, SIGNAL(selectionChanged()),
		this, SIGNAL(selectionChanged()));

	if( item->isSelected() )
		emit selectionChanged();

	return item;
}

InfoListWidgetItem* InfoListWidget::takeItem(InfoListWidgetItem *item)
{
	if( !mItems.contains(item) )
		return 0;

	mItems.removeAt( mItems.indexOf(item) );
	mLayout->removeWidget(item);

	item->mInfoListWidget = 0;

	disconnect(item, SIGNAL(clicked()), this, SLOT(handleItemClicked()));
	disconnect(item, SIGNAL(doubleClicked()),
		this, SLOT(handleItemDoubleClicked()));
	disconnect(item, SIGNAL(selectionChanged()),
		this, SIGNAL(selectionChanged()));

	if( item->isSelected() )
		emit selectionChanged();

	return item;
}

int InfoListWidget::count() const
{
	return mItems.count();
}

QList<InfoListWidgetItem*> InfoListWidget::selectedItems() const
{
	QList<InfoListWidgetItem*> selected;

	foreach(InfoListWidgetItem *item, mItems)
	{
		if( item->isSelected() )
			selected << item;
	}

	return selected;
}

void InfoListWidget::setCurrentRow(int r)
{
	if(mItems.count() >= r)
		return;

	foreach(InfoListWidgetItem *item, mItems)
		item->setSelected(false);

	mItems.at(r)->setSelected(true);
}

void InfoListWidget::setCurrentItem(InfoListWidgetItem *item)
{
	if( !mItems.contains(item) )
		return;

	foreach(InfoListWidgetItem *i, mItems)
		i->setSelected(false);

	item->setSelected(true);
}

void InfoListWidget::clear()
{
	foreach(InfoListWidgetItem *item, mItems)
	{
		mLayout->removeWidget(item);
		delete item;
	}

	mItems.clear();

	emit selectionChanged();
}

void InfoListWidget::handleItemClicked()
{
	InfoListWidgetItem *item = qobject_cast<InfoListWidgetItem*>( sender() );
	if(!item)
		return;

	switch(mSelectionMode)
	{
		case InfoListWidget::SingleSelection:
		{
			bool blocked = item->blockSignals(true);
			foreach(InfoListWidgetItem *i, mItems)
				i->setSelected(false);

			item->blockSignals(blocked);
			item->setSelected(true);
			break;
		}
		case InfoListWidget::MultiSelection:
		{
			item->setSelected( !item->isSelected() );
			break;
		}
		default:
			break;
	}

	emit itemClicked(item);
}

void InfoListWidget::handleItemDoubleClicked()
{
	InfoListWidgetItem *item = qobject_cast<InfoListWidgetItem*>( sender() );
	if(!item)
		return;

	emit itemDoubleClicked(item);
}

void InfoListWidget::setSortHeaders(const QString& name1, const QString& name2,
		const QString& name3, const QString& name4)
{
	ui.sort1->setText(name1 + ' ');
	ui.sort2->setText(name2 + ' ');
	ui.sort3->setText(name3 + ' ');
	ui.sort4->setText(name4 + ' ');

	ui.sort1->setVisible( !name1.isEmpty() );
	ui.sort2->setVisible( !name2.isEmpty() );
	ui.sort3->setVisible( !name3.isEmpty() );
	ui.sort4->setVisible( !name4.isEmpty() );
	ui.sortArrow1->setVisible( !name1.isEmpty() );
	ui.sortArrow2->setVisible( !name2.isEmpty() );
	ui.sortArrow3->setVisible( !name3.isEmpty() );
	ui.sortArrow4->setVisible( !name4.isEmpty() );
}

void InfoListWidget::setSortFunction1(InfoItemSortFunc func)
{
	mSortFunctions[0] = func;
}

void InfoListWidget::setSortFunction2(InfoItemSortFunc func)
{
	mSortFunctions[1] = func;
}

void InfoListWidget::setSortFunction3(InfoItemSortFunc func)
{
	mSortFunctions[2] = func;
}

void InfoListWidget::setSortFunction4(InfoItemSortFunc func)
{
	mSortFunctions[3] = func;
}

bool InfoListWidget::lessThen(const InfoListWidgetItem *item1,
	const InfoListWidgetItem *item2)
{
	for(int i = 0; i < 4; i++)
	{
		int index = mSortOrder.at(i);

		QString a = item1->textAt(index);
		QString b = item2->textAt(index);

		if(a == b)
			continue;

		if(mSortDirection[i] == -1) // Descending
			return !mSortFunctions[i](item1, item2);
		else // 1 - Ascending
			return mSortFunctions[i](item1, item2);
	}

	// They are exactly the same, so return false
	return false;
}

bool InfoListWidget::isSortingEnabled() const
{
	return mSortingEnabled;
}

void InfoListWidget::setSortingEnabled(bool enable)
{
	mSortingEnabled = enable;
}

InfoListWidget::SelectionMode InfoListWidget::selectionMode() const
{
	return mSelectionMode;
}

void InfoListWidget::setSelectionMode(SelectionMode mode)
{
	if(mode == SingleSelection)
	{
		QList<InfoListWidgetItem*> items = selectedItems();
		if(items.count() > 1)
		{
			items.removeFirst();
			foreach(InfoListWidgetItem *item, items)
				item->setSelected(false);
		}
	}

	mSelectionMode = mode;
}

void InfoListWidget::sort()
{
	mergeSort(mItems);

	foreach(InfoListWidgetItem *item, mItems)
		mLayout->removeWidget(item);

	foreach(InfoListWidgetItem *item, mItems)
		mLayout->addWidget(item);
}

void InfoListWidget::mergeSort(QList<InfoListWidgetItem*>& list)
{
	int list_count = list.count();

	if(list_count < 2)
		return;

	// Use insertion sort on small lists (faster)
	if(list_count < 64)
	{
		InfoListWidgetItem *temp;

		for(int i = 1; i < list_count; i++)
		{
			temp = list[i];

			int j;
			for(j = i - 1; j >= 0 && lessThen(temp, list[j]); j--)
				list[j + 1] = list[j];

			list[j + 1] = temp;
		}

		return;
	}

	int half = list_count / 2;

    QList<InfoListWidgetItem*> half1 = list.mid(0, half);
    QList<InfoListWidgetItem*> half2 = list.mid(half);
	QList<InfoListWidgetItem*> temp;

	mergeSort(half1);
	mergeSort(half2);

	int i = 0, j = 0;
	int count2 = list_count - half;
	while(i < half && j < count2)
	{
		if( lessThen(half1[i], half2[j]) )
			temp << half1[i++];
		else
			temp << half2[j++];
	}

	while(i < half)
		temp << half1[i++];

	while(j < count2)
		temp << half2[j++];

	list = temp;
}

QLabel* InfoListWidget::arrowAt(int index)
{
	switch(index)
	{
		case 0:
			return ui.sortArrow1;
			break;
		case 1:
			return ui.sortArrow2;
			break;
		case 2:
			return ui.sortArrow3;
			break;
		case 3:
			return ui.sortArrow4;
			break;
		default:
			break;
	}

	return 0;
}

void InfoListWidget::toggleSort(int num)
{
	if(num < 0 || num > 3)
		return;

	int last = mSortOrder.first();
	if(mSortDirection[last] == -1) // Descending
		arrowAt(last)->setPixmap(mIconDown1);
	else // 1 - Ascending
		arrowAt(last)->setPixmap(mIconUp1);

	int index = mSortOrder.indexOf(num);
	mSortOrder.prepend( mSortOrder.takeAt(index) );
	mSortFunctions.prepend( mSortFunctions.takeAt(index) );
	mSortDirection.prepend(mSortDirection.takeAt(index) == -1 ? 1 : -1);

	if(mSortDirection.first() == -1) // Descending
		arrowAt(num)->setPixmap(mIconDown2);
	else // 1 - Ascending
		arrowAt(num)->setPixmap(mIconUp2);

	// Now sort the items
	sort();
}

void InfoListWidget::toggleSort1()
{
	toggleSort(0);
}

void InfoListWidget::toggleSort2()
{
	toggleSort(1);
}

void InfoListWidget::toggleSort3()
{
	toggleSort(2);
}

void InfoListWidget::toggleSort4()
{
	toggleSort(3);
}
