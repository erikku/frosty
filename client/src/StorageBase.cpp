/******************************************************************************\
*  client/src/StorageBase.cpp                                                  *
*  Copyright (C) 2009 John Eric Martin <john.eric.martin@gmail.com>            *
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

#include "StorageBase.h"

#include <QtCore/QTimer>
#include <QtCore/QMimeData>

#include <QtGui/QPushButton>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QDrag>

#include "ajax.h"
#include "json.h"
#include "Taskbar.h"
#include "Settings.h"
#include "AddDevil.h"
#include "DevilCache.h"
#include "FusionChart.h"
#include "DevilProperties.h"

StorageBase::StorageBase(QWidget *parent_widget) : QWidget(parent_widget),
	mEditable(false), mLoaded(false), mMarked(false), mSyncTimer(0),
	mAddDevil(false), mFusionChart(0), mFuseButton(0), mPropertiesButton(0),
	mDismissButton(0), mAddButton(0), mDuplicateButton(0), mExtractButton(0)
{
	// Nothing to see here
}

bool StorageBase::isEditable() const
{
	return mEditable;
}

void StorageBase::setEditable(bool editable)
{
	mEditable = editable;
}

int StorageBase::count() const
{
	int max = capacity();
	int used = 0;

	for(int i = 0; i < max; i++)
	{
		if( !devilAt(i).isEmpty() )
			used++;
	}

	return used;
}

void StorageBase::add()
{
	int index = activeIndex();
	if(!mEditable || index < 0)
		return;

	if( !devilAt(index).isEmpty() )
		return;

	Q_ASSERT(mAddDevil);

	mAddDevil->add(index);
}

void StorageBase::dismiss()
{
	int index = activeIndex();
	if(!mEditable || index < 0)
		return;

	DevilData devil_data = devilAt(index);
	if( devil_data.isEmpty() )
		return;

	// TODO: Ask if you are sure (include the devil name)

	clearAt(index);
	selectionChanged();
	updateCount();
}

void StorageBase::extract()
{
	int index = activeIndex();
	if(!mEditable || index < 0)
		return;

	DevilData devil_data = devilAt(index);
	if( devil_data.isEmpty() )
		return;

	QVariantList parent_data = devil_data.value("parents").toList();
	int parent_count = parent_data.count();

	if( ( capacity() - count() ) < parent_count )
		return;

	QListIterator<QVariant> it(parent_data);
	while( it.hasNext() )
	{
		DevilData parent_devil = it.next().toMap();
		setAt(nextFreeIndex(), parent_devil);
	}
}

void StorageBase::duplicate()
{
	int free_index = nextFreeIndex();
	if(!mEditable || free_index < 0)
		return;

	int index = activeIndex();
	if(index < 0)
		return;

	DevilData devil_data = devilAt(index);
	if( devil_data.isEmpty() )
		return;

	setAt(free_index, devil_data);
}

void StorageBase::properties()
{
	int index = activeIndex();
	if(index < 0)
		return;

	DevilData devil_data = devilAt(index);
	if( devil_data.isEmpty() )
		return;

	DevilProperties *prop = DevilProperties::getSingletonPtr();

	prop->setActiveDevil(this, index, devil_data);
	prop->show();
	prop->activateWindow();
	prop->raise();
}

void StorageBase::startFusion()
{
	if(!mEditable)
		return;

	int max = capacity();

	QList<DevilData> devils;
	for(int i = 0; i < max; i++)
		devils << devilAt(i);

	DevilProperties::getSingletonPtr()->hide();

	Q_ASSERT(mFusionChart);

	mFusionChart->loadDevils(this, devils);
}

void StorageBase::sync()
{
	if(!mEditable)
		return;

	QVariantList sync_actions = syncData();
	foreach(QVariant action, sync_actions)
		ajax::getSingletonPtr()->request(settings->url(), action);

	// Stop the timer now
	mSyncTimer->stop();
	mMarked = false;
}

void StorageBase::markDirty()
{
	if(!mLoaded || mMarked || !mEditable)
		return;

	mMarked = true;

	mSyncTimer->start(60000); // 60 seconds

	Taskbar::getSingletonPtr()->notifyDirty( syncUserData() );
}

void StorageBase::loadDevils()
{
	QVariantList load_actions = loadData();
	foreach(QVariant action, load_actions)
		ajax::getSingletonPtr()->request(settings->url(), action);
}

void StorageBase::selectionChanged()
{
	bool good = false;

	int index = activeIndex();
	int free_slots = capacity() - count();

	DevilData devil_data;
	if(index >= 0)
	{
		devil_data = devilAt(index);
		good = !devil_data.isEmpty();

		DevilProperties *prop = DevilProperties::getSingletonPtr();

		if(prop->isVisible() && good)
			prop->setActiveDevil(this, index, devil_data);
		else
			prop->hide();

		if(good && mExtractButton && mEditable)
		{
			int parent_count = devil_data.value("parents").toList().count();
			if(parent_count > 0)
				mExtractButton->setEnabled(free_slots >= parent_count );
			else
				mExtractButton->setEnabled(false);
		}
		else if(mExtractButton)
		{
			mExtractButton->setEnabled(false);
		}
	}
	else if(mExtractButton)
	{
		mExtractButton->setEnabled(false);
	}

	if(mPropertiesButton)
		mPropertiesButton->setEnabled(good);
	if(mDismissButton)
		mDismissButton->setEnabled(good && mEditable);

	if(mAddButton)
		mAddButton->setEnabled(index >= 0 && !good && mEditable);
	if(mDuplicateButton)
		mDuplicateButton->setEnabled(good && free_slots >= 1 && mEditable);
}

void StorageBase::indexDoubleClicked(int idx)
{
	int index = idx;
	if(index < 0)
		index = activeIndex();

	if(index < 0)
		return;

	DevilData devil_data = devilAt(index);
	if( devil_data.isEmpty() )
	{
		Q_ASSERT(mAddDevil);

		// Empty slot, so add a devil
		mAddDevil->add(index);
	}
	else
	{
		DevilProperties *prop = DevilProperties::getSingletonPtr();

		prop->setActiveDevil(this, index, devil_data);
		prop->show();
		prop->activateWindow();
		prop->raise();
	}
}

void StorageBase::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();

	QString user_data = result.value("user_data").toString();
	if(loadUserData() != user_data)
		return;

	int max = capacity();

	QList<DevilData> devils = readLoadData(result);
	if(devils.count() != max)
		return;

	int index = -1;
	foreach(DevilData devil_data, devils)
	{
		index++;
		if( devil_data.isEmpty() )
			continue;

		setAt(index, devil_data);
	}

	updateCount();
	setEnabled(true);
	mLoaded = true;
}

QList<DevilData> StorageBase::readLoadData(const QVariantMap& result) const
{
	QVariantList list = result.value("devils").toList();

	QList<DevilData> devils;
	QListIterator<QVariant> it(list);
	while( it.hasNext() )
		devils << json::parse( it.next().toString() ).toMap();

	return devils;
}

void StorageBase::initStorage(QPushButton *addButton, QPushButton *dismissButton,
	QPushButton *propertiesButton, QPushButton *duplicateButton,
	QPushButton *extractButton, QPushButton *fuseButton)
{
	mAddButton = addButton;
	mDismissButton = dismissButton;
	mPropertiesButton = propertiesButton;
	mDuplicateButton = duplicateButton;
	mExtractButton = extractButton;
	mFuseButton = fuseButton;

	setEnabled(false);
	setAcceptDrops(true);

	if(mPropertiesButton)
	{
		connect(mPropertiesButton, SIGNAL(clicked(bool)),
			this, SLOT(properties()));
	}
	if(mDismissButton)
	{
		connect(mDismissButton, SIGNAL(clicked(bool)),
			this, SLOT(dismiss()));
	}
	if(mFuseButton)
	{
		connect(mFuseButton, SIGNAL(clicked(bool)),
			this, SLOT(startFusion()));
	}
	if(mAddButton)
	{
		connect(mAddButton, SIGNAL(clicked(bool)),
			this, SLOT(add()));
	}
	if(mDuplicateButton)
	{
		connect(mDuplicateButton, SIGNAL(clicked(bool)),
			this, SLOT(duplicate()));
	}
	if(mExtractButton)
	{
		connect(mExtractButton, SIGNAL(clicked(bool)),
			this, SLOT(extract()));
	}

	DevilCache *cache = DevilCache::getSingletonPtr();
	connect(cache, SIGNAL(cacheReady()), this, SLOT(loadDevils()));
	cache->fillCache();

	ajax::getSingletonPtr()->subscribe(this);

	mSyncTimer = new QTimer;
	mSyncTimer->setSingleShot(true);

	connect(mSyncTimer, SIGNAL(timeout()), this, SLOT(sync()));

	mAddDevil = new AddDevil(this);
	mFusionChart = new FusionChart(this);

	connect(mAddDevil, SIGNAL(devilSelected(int, const DevilData&)),
		this, SLOT(setAt(int, const DevilData&)));

	selectionChanged();
}

int StorageBase::nextFreeIndex() const
{
	int max = capacity();
	for(int i = 0; i < max; i++)
	{
		if( devilAt(i).isEmpty() )
			return i;
	}

	return -1;
}

QPoint StorageBase::hotspot(int index) const
{
	return ( mDragStartPosition - indexPosition(index) );
}

void StorageBase::mouseDoubleClickEvent(QMouseEvent *evt)
{
	if(evt->button() != Qt::LeftButton)
		return;

	int index = indexAt( evt->pos() );
	if(index < 0)
		return;

	indexDoubleClicked(index);
}

void StorageBase::mousePressEvent(QMouseEvent *evt)
{
	if(evt->buttons() & Qt::LeftButton)
		mDragStartPosition = evt->pos();
}

void StorageBase::mouseMoveEvent(QMouseEvent *evt)
{
	if( !(evt->buttons() & Qt::LeftButton) )
		return;

	if( (evt->pos() - mDragStartPosition).manhattanLength()
		< QApplication::startDragDistance() )
			return;

	int index = indexAt(mDragStartPosition);
	if(index < 0)
		return;

	DevilData devil_data = devilAt(index);
	if( devil_data.isEmpty() )
		return;

	DevilData devil_info = DevilCache::getSingletonPtr()->devilByID(
		devil_data.value("id").toInt() );

	QPixmap pixmap( QString("icons/devils/icon_%1.png").arg(
		devil_info.value("icon").toString() ) );

	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	dataStream << devil_data << index;

	QMimeData *mimeData = new QMimeData;
	mimeData->setData("application/x-devil", itemData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(pixmap);
	drag->setHotSpot( hotspot(index) );
	drag->exec();

	updateCount();
}

void StorageBase::dragEnterEvent(QDragEnterEvent *evt)
{
	if( evt->mimeData()->hasFormat("application/x-devil") && mEditable )
		evt->acceptProposedAction();
}

void StorageBase::dragMoveEvent(QDragMoveEvent *evt)
{
	if(!mEditable)
	{
		evt->ignore();
		return;
	}

	bool sourceThis = (evt->source() == this);

	StorageBase *storage = 0;
	if( evt->source()->inherits("StorageBase") && !sourceThis )
		storage = qobject_cast<StorageBase*>( evt->source() );

	int index = indexAt( evt->pos() );

	if( evt->mimeData()->hasFormat("application/x-devil") && index >= 0 )
	{
		DevilData devil_data = devilAt(index);

		if( devil_data.isEmpty() || ( storage && storage->isEditable() ) )
		{
			evt->acceptProposedAction();
		}
		else if(sourceThis)
		{
			QByteArray itemData = evt->mimeData()->data("application/x-devil");
			QDataStream dataStream(&itemData, QIODevice::ReadOnly);

			int source_index = -1;
			DevilData source_devil;

			dataStream >> source_devil >> source_index;

			if(source_index == index)
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

void StorageBase::dropEvent(QDropEvent *evt)
{
	if(!mEditable)
	{
		evt->ignore();
		return;
	}

	bool sourceThis = (evt->source() == this);

	StorageBase *storage = 0;
	if( evt->source()->inherits("StorageBase") && !sourceThis )
		storage = qobject_cast<StorageBase*>( evt->source() );

	int index = indexAt( evt->pos() );

	if( !evt->mimeData()->hasFormat("application/x-devil") || index < 0 )
	{
		evt->ignore();
		return;
	}

	DevilData old_devil = devilAt(index);

	QByteArray itemData = evt->mimeData()->data("application/x-devil");
	QDataStream dataStream(&itemData, QIODevice::ReadOnly);

	DevilData source_devil;
	dataStream >> source_devil;

	// Move/swap instead of just copy
	if( storage && storage->isEditable() )
	{
		int source_index = -1;
		dataStream >> source_index;

		storage->clearAt(source_index);

		if( !old_devil.isEmpty() )
			storage->setAt(source_index, old_devil);
	}
	else if(sourceThis)
	{
		int source_index = -1;
		dataStream >> source_index;

		if(source_index == index)
		{
			evt->ignore();
			return;
		}

		clearAt(source_index);

		if( !old_devil.isEmpty() )
			setAt(source_index, old_devil);
	}
	else if( !old_devil.isEmpty() )
	{
		evt->ignore();
		return;
	}

	setAt(index, source_devil);

	evt->acceptProposedAction();

	selectionChanged();
	updateCount();
	markDirty();
}

void StorageBase::contextMenuEvent(QContextMenuEvent *evt)
{
	// TODO: Add a context menu
	Q_UNUSED(evt);
}
