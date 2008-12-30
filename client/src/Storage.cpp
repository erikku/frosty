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
#include "Settings.h"
#include "AddDevil.h"
#include "Taskbar.h"
#include "COMP.h"
#include "ajax.h"
#include "json.h"

#include <QtCore/QTimer>
#include <QtCore/QMimeData>
#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>

Storage::Storage(QWidget *parent_widget) : QWidget(parent_widget),
	mLoaded(false), mMarked(false)
{
	ui.setupUi(this);
	setEnabled(false);

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

	QPixmap blank(":/blank.png");
	foreach(QLabel *slot, mSlots)
	{
		slot->setPixmap(blank);
		mData << QVariantMap();
	}

	setAcceptDrops(true);

	DevilCache *cache = DevilCache::getSingletonPtr();
	connect(cache, SIGNAL(cacheReady()), this, SLOT(loadDevils()));
	cache->fillCache();

	ajax::getSingletonPtr()->subscribe(this);

	mSyncTimer = new QTimer;
	mSyncTimer->setSingleShot(true);

	connect(mSyncTimer, SIGNAL(timeout()), this, SLOT(sync()));

	mAddDevil = new AddDevil(this);

	connect(mAddDevil, SIGNAL(devilSelected(int, const QVariantMap&)),
		this, SLOT(setAt(int, const QVariantMap&)));
}

void Storage::loadDevils()
{
	QVariantMap action;
	action["action"] = "simulator_load_storage";
	action["user_data"] = "simulator_load_storage";

	ajax::getSingletonPtr()->request(settings->url(), action);
}

void Storage::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();

	if(result.value("user_data").toString() != "simulator_load_storage")
		return;

	int max = mSlots.count();

	QVariantList devils = result.value("devils").toList();
	if(devils.count() != max)
		return;

	for(int i = 0; i < max; i++)
	{
		QVariantMap devil = json::parse(devils.at(i).toString()).toMap();
		if( devil.isEmpty() )
			continue;

		setAt(i, devil);
	}

	updateCount();
	setEnabled(true);
	mLoaded = true;
}

void Storage::mouseDoubleClickEvent(QMouseEvent *evt)
{
	QLabel *icon = qobject_cast<QLabel*>( childAt( evt->pos() ) );
	if(!icon)
		return;

	int index = mSlots.indexOf(icon);

	if( mData.at(index).isEmpty() )
	{
		// Empty slot, so add a devil
		mAddDevil->add(index);
	}
	else
	{
		// TODO: The slot exists, so show the devil's properties
	}
}

void Storage::mousePressEvent(QMouseEvent *evt)
{
	if(evt->buttons() & Qt::LeftButton)
		mDragStartPosition = evt->pos();
}

void Storage::mouseMoveEvent(QMouseEvent *evt)
{
	// TODO: Move the event pos() from here, into the mousePressEvent

	if( !(evt->buttons() & Qt::LeftButton) )
		return;

	if( (evt->pos() - mDragStartPosition).manhattanLength()
		< QApplication::startDragDistance() )
			return;

	QLabel *icon = qobject_cast<QLabel*>( childAt( evt->pos() ) );
	if(!icon)
		return;

	int index = mSlots.indexOf(icon);
	if(index < 0)
		return;

	QVariantMap devil = mData.at(index);
	if( devil.isEmpty() )
		return;

	QPixmap pixmap = *icon->pixmap();

	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	dataStream << devil << mSlots.indexOf(icon);

	QMimeData *mimeData = new QMimeData;
	mimeData->setData("application/x-devil", itemData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(pixmap);
	drag->setHotSpot(evt->pos() - icon->pos());
	drag->exec();
	updateCount();
}

void Storage::clearAt(int index)
{
	if(index < 0 || index >= mSlots.count())
		return;

	mSlots.at(index)->setPixmap( QPixmap(":/blank.png") );
	mData[index] = QVariantMap();

	markDirty();
	updateCount();
}

void Storage::setAt(int index, const QVariantMap& devil)
{
	if(index < 0 || index >= mSlots.count())
		return;

	DevilCache *cache = DevilCache::getSingletonPtr();

	QString tooltip = cache->devilToolTip(devil);

	QVariantMap devil_data = cache->devilByID( devil.value("id").toInt() );

	QPixmap icon( QString("icons/devils/icon_%1.png").arg(
		devil_data.value("icon").toString() ) );

	QLabel *slot = mSlots.at(index);
	slot->setPixmap(icon);
	slot->setToolTip(tooltip);
	mData[index] = devil;

	markDirty();
	updateCount();
}

void Storage::dragEnterEvent(QDragEnterEvent *evt)
{
	if( evt->mimeData()->hasFormat("application/x-devil") )
		evt->acceptProposedAction();
}

void Storage::dragMoveEvent(QDragMoveEvent *evt)
{
	COMP *comp = 0;
	if( evt->source()->inherits("COMP") )
		comp = qobject_cast<COMP*>( evt->source() );

	bool sourceThis = (evt->source() == this);

	QLabel *icon = qobject_cast<QLabel*>( childAt( evt->pos() ) );
	int index = mSlots.indexOf(icon);

	if( evt->mimeData()->hasFormat("application/x-devil") && icon )
	{
		if(mData.at(index).isEmpty() || comp)
		{
			evt->acceptProposedAction();
		}
		else if(sourceThis)
		{
			QByteArray itemData = evt->mimeData()->data("application/x-devil");
			QDataStream dataStream(&itemData, QIODevice::ReadOnly);

			int src_index = -1;
			QVariantMap devil;
			dataStream >> devil >> src_index;

			if(src_index == index)
				evt->ignore();
			else
				evt->acceptProposedAction();
		}
		else
		{
			evt->ignore();
		}
	}
	else
	{
		evt->ignore();
	}
}

void Storage::dropEvent(QDropEvent *evt)
{
	COMP *comp = 0;
	if( evt->source()->inherits("COMP") )
		comp = qobject_cast<COMP*>( evt->source() );

	bool sourceThis = (evt->source() == this);

	QLabel *icon = qobject_cast<QLabel*>( childAt( evt->pos() ) );
	int index = mSlots.indexOf(icon);

	if( !evt->mimeData()->hasFormat("application/x-devil") || !icon )
	{
		evt->ignore();
		return;
	}

	QByteArray itemData = evt->mimeData()->data("application/x-devil");
	QDataStream dataStream(&itemData, QIODevice::ReadOnly);

	QVariantMap devil;
	dataStream >> devil;

	// Move/swap instead of just copy
	if(comp)
	{
		int src_index = -1;
		dataStream >> src_index;

		comp->clearAt(src_index);

		QVariantMap old_devil = mData.at(index);
		if( !old_devil.isEmpty() )
			comp->setAt(src_index, old_devil);
	}
	else if(sourceThis)
	{
		int src_index = -1;
		dataStream >> src_index;

		if(src_index == index)
		{
			evt->ignore();
			return;
		}

		mData[src_index] = mData[index];
		mSlots.at(src_index)->setPixmap( *(mSlots.at(index)->pixmap()) );
	}
	else if( !mData.at(index).isEmpty() )
	{
		evt->ignore();
		return;
	}

	setAt(index, devil);

	evt->acceptProposedAction();

	updateCount();
	markDirty();
}

void Storage::markDirty()
{
	if(!mLoaded || mMarked)
		return;

	mMarked = true;

	mSyncTimer->start(60000); // 60 seconds

	Taskbar::getSingletonPtr()->notifyDirty("simulator_sync_storage");
}

void Storage::sync()
{
	QVariantMap action;
	action["action"] = "simulator_sync_storage";
	action["user_data"] = "simulator_sync_storage";

	int max = mSlots.count();

	QVariantList devils;
	for(int i = 0; i < max; i++)
		devils << json::toJSON(mData[i]);

	action["devils"] = devils;

	ajax::getSingletonPtr()->request(settings->url(), action);

	// Stop the timer now
	mSyncTimer->stop();
	mMarked = false;
}

void Storage::updateCount()
{
	int count = 0;
	int max = mSlots.count();

	for(int i = 0; i < max; i++)
	{
		if( !mData.at(i).isEmpty() )
			count++;
	}

	ui.devilCount->setText(tr("%1/%2").arg(count).arg(max));
}
