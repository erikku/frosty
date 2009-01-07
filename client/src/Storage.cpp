/******************************************************************************\
*  client/src/Storage.cpp                                                      *
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

#include "Storage.h"
#include "DevilCache.h"
#include "json.h"

Storage::Storage(QWidget *parent_widget) : StorageBase(parent_widget),
	mActiveIndex(-1)
{
	ui.setupUi(this);
	setEditable(true);

	setWindowTitle( tr("%1 - Devil Storage").arg(
		tr("Absolutely Frosty") ) );

	mSlots << ui.devil1 << ui.devil2 << ui.devil3 << ui.devil4 << ui.devil5
		 << ui.devil6  << ui.devil7 << ui.devil8 << ui.devil9 << ui.devil10
		 << ui.devil11 << ui.devil12 << ui.devil13 << ui.devil14 << ui.devil15
		 << ui.devil16 << ui.devil17 << ui.devil18 << ui.devil19 << ui.devil20
		 << ui.devil21 << ui.devil22 << ui.devil23 << ui.devil24 << ui.devil25
		 << ui.devil26 << ui.devil27 << ui.devil28 << ui.devil29 << ui.devil30
		 << ui.devil31 << ui.devil32 << ui.devil33 << ui.devil34 << ui.devil35
		 << ui.devil36 << ui.devil37 << ui.devil38 << ui.devil39 << ui.devil40
		 << ui.devil41 << ui.devil42 << ui.devil43 << ui.devil44 << ui.devil45
		 << ui.devil46 << ui.devil47 << ui.devil48 << ui.devil49 << ui.devil50;

	QPixmap blank(":/border.png");
	foreach(QLabel *slot, mSlots)
	{
		slot->setPixmap(blank);
		mData << DevilData();
	}

	initStorage(0, 0, 0);
}

int Storage::capacity() const
{
	return 50;
}

DevilData Storage::devilAt(int index) const
{
	Q_ASSERT(index < capacity() || index >= 0);
	if(index >= capacity() || index < 0)
		return DevilData();

	return mData.value(index);
}

int Storage::activeIndex() const
{
	// Items in this storage device are not selectable, so only return an
	// index that was set by code.
	return mActiveIndex;
}

void Storage::updateCount()
{
	int used = count();
	int max = capacity();

	ui.devilCount->setText(tr("%1/%2").arg(used).arg(max));
}

void Storage::clearAt(int index)
{
	Q_ASSERT(index < capacity() || index >= 0);
	if(index >= capacity() || index < 0)
		return;

	mSlots.at(index)->setPixmap( QPixmap(":/border.png") );
	mData[index] = DevilData();

	markDirty();
	updateCount();
}

void Storage::setAt(int index, const DevilData& devil)
{
	Q_ASSERT(index < capacity() || index >= 0);
	if(index >= capacity() || index < 0)
		return;

	DevilCache *cache = DevilCache::getSingletonPtr();

	QString tooltip = cache->devilToolTip(devil);

	DevilData devil_data = cache->devilByID( devil.value("id").toInt() );

	QPixmap icon( QString("icons/devils/icon_%1.png").arg(
		devil_data.value("icon").toString() ) );

	QLabel *slot = mSlots.at(index);
	slot->setPixmap(icon);
	slot->setToolTip(tooltip);
	mData[index] = devil;

	markDirty();
	updateCount();
}

void Storage::setActiveIndex(int index)
{
	Q_ASSERT(index < capacity() || index >= 0);
	if(index >= capacity() || index < 0)
		return;

	mActiveIndex = index;
}

int Storage::indexAt(const QPoint& p) const
{
	QLabel *icon = qobject_cast<QLabel*>( childAt(p) );
	if(!icon)
		return -1;

	return mSlots.indexOf(icon);
}

QPoint Storage::indexPosition(int index) const
{
	Q_ASSERT(index < capacity() || index >= 0);
	if(index >= capacity() || index < 0)
		return QPoint(0, 0);

	return mSlots.at(index)->pos();
}

QString Storage::loadUserData() const
{
	return "simulator_load_storage";
}

QVariantList Storage::loadData() const
{
	QVariantMap action;
	action["action"] = "simulator_load_storage";
	action["user_data"] = "simulator_load_storage";

	return QVariantList() << action;
}

QString Storage::syncUserData() const
{
	return "simulator_sync_storage";
}

QVariantList Storage::syncData() const
{
	QVariantMap action;
	action["action"] = "simulator_sync_storage";
	action["user_data"] = "simulator_sync_storage";

	int max = capacity();

	QVariantList devils;
	for(int i = 0; i < max; i++)
		devils << json::toJSON(mData[i]);

	action["devils"] = devils;

	return QVariantList() << action;
}
