/******************************************************************************\
*  client/src/COMP.cpp                                                         *
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

#include "COMP.h"
#include "Storage.h"
#include "Taskbar.h"
#include "Settings.h"
#include "AddDevil.h"
#include "DevilCache.h"
#include "DevilProperties.h"
#include "IconListWidgetItem.h"
#include "json.h"
#include "ajax.h"

#include <QtCore/QTimer>
#include <QtGui/QMessageBox>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>

COMP::COMP(QWidget *parent_widget) : QWidget(parent_widget),
	mLoaded(false), mMarked(false)
{
	ui.setupUi(this);
	setEnabled(false);

	setWindowTitle( tr("%1 - COMP").arg(
		tr("Absolutely Frosty") ) );

	connect(ui.devilList, SIGNAL(selectionChanged()),
		this, SLOT(selectionChanged()));
	connect(ui.devilList, SIGNAL(itemDoubleClicked(IconListWidgetItem*)),
		this, SLOT(itemDoubleClicked(IconListWidgetItem*)));
	connect(ui.propButton, SIGNAL(clicked(bool)),
		this, SLOT(properties()));
	connect(ui.dismissButton, SIGNAL(clicked(bool)),
		this, SLOT(dismiss()));

	QIcon blank(":/blank.png");

	// Fill in blanks
	for(int i = 0; i < 8; i++)
	{
		IconListWidgetItem *item = new IconListWidgetItem(blank);
		ui.devilList->addItem(item);
	}

	selectionChanged();
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

void COMP::selectionChanged()
{
	bool good = false;

	QList<IconListWidgetItem*> items = ui.devilList->selectedItems();
	if( items.count() )
	{
		IconListWidgetItem *item = items.first();
		good = !item->upperText().isEmpty();

		DevilProperties *prop = DevilProperties::getSingletonPtr();

		if( good && prop->isVisible() )
		{
			prop->setActiveDevil( this,
				ui.devilList->row(item), item->data().toMap() );
		}
		else
		{
			prop->hide();
		}
	}

	ui.propButton->setEnabled(good);
	ui.dismissButton->setEnabled(good);
}

IconListWidgetItem* COMP::itemAt(const QPoint& p)
{
	QObject *child = childAt(p);
	if(!child)
		return 0;

	bool isItem = false;
	do
	{
		if( !child->inherits("IconListWidgetItem") )
			continue;

		isItem = true;
		break;
	} while( (child = child->parent()) != this && child );

	if(isItem)
		return qobject_cast<IconListWidgetItem*>(child);

	return 0;
}

void COMP::mousePressEvent(QMouseEvent *evt)
{
	if(evt->buttons() & Qt::LeftButton)
		mDragStartPosition = evt->pos();
}

void COMP::mouseMoveEvent(QMouseEvent *evt)
{
	if( !(evt->buttons() & Qt::LeftButton) )
		return;

	if( (evt->pos() - mDragStartPosition).manhattanLength()
		< QApplication::startDragDistance() )
			return;

	IconListWidgetItem *item = itemAt( mapFromGlobal( evt->globalPos() ) );
	if(!item)
		return;

	QVariantMap devil = item->data().toMap();
	if( devil.isEmpty() )
		return;

	// Release the mouse from the item so the drag works right
	QMouseEvent release_event(QEvent::MouseMove, QPoint(-1, -1),
		QPoint(-1, -1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
	qApp->sendEvent(item, &release_event);

	QPixmap pixmap = item->icon().pixmap(32, 32);

	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	dataStream << devil << ui.devilList->row(item);

	QMimeData *mimeData = new QMimeData;
	mimeData->setData("application/x-devil", itemData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(pixmap);
	drag->setHotSpot(QPoint(16, 16));
	drag->exec();
	updateCount();
}

void COMP::dragEnterEvent(QDragEnterEvent *evt)
{
	if( evt->mimeData()->hasFormat("application/x-devil") )
		evt->acceptProposedAction();
}

void COMP::dragMoveEvent(QDragMoveEvent *evt)
{
	Storage *storage = 0;
	if( evt->source()->inherits("Storage") )
		storage = qobject_cast<Storage*>( evt->source() );

	bool sourceThis = (evt->source() == this);

	IconListWidgetItem *item = itemAt( evt->pos() );
	if( evt->mimeData()->hasFormat("application/x-devil") && item )
	{
		if(item->upperText().isEmpty() || storage)
		{
			evt->acceptProposedAction();
		}
		else if(sourceThis)
		{
			QByteArray itemData = evt->mimeData()->data("application/x-devil");
			QDataStream dataStream(&itemData, QIODevice::ReadOnly);

			int index = -1;
			QVariantMap devil;
			dataStream >> devil >> index;

			if(ui.devilList->row(item) == index)
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

void COMP::dropEvent(QDropEvent *evt)
{
	Storage *storage = 0;
	if( evt->source()->inherits("Storage") )
		storage = qobject_cast<Storage*>( evt->source() );

	bool sourceThis = (evt->source() == this);

	IconListWidgetItem *item = itemAt( evt->pos() );
	if( !evt->mimeData()->hasFormat("application/x-devil") || !item )
	{
		evt->ignore();
		return;
	}

	QByteArray itemData = evt->mimeData()->data("application/x-devil");
	QDataStream dataStream(&itemData, QIODevice::ReadOnly);

	QVariantMap devil;
	dataStream >> devil;

	// Move/swap instead of just copy
	if(storage)
	{
		int index = -1;
		dataStream >> index;

		storage->clearAt(index);

		QVariantMap old_devil = item->data().toMap();
		if( !old_devil.isEmpty() )
			storage->setAt(index, old_devil);
	}
	else if(sourceThis)
	{
		int index = -1;
		dataStream >> index;

		if(ui.devilList->row(item) == index)
		{
			evt->ignore();
			return;
		}

		IconListWidgetItem *src = ui.devilList->itemAt(index);
		Q_ASSERT(src);

		src->setUpperText( item->upperText() );
		src->setLowerText( item->lowerText() );
		src->setIcon( item->icon() );
		src->setData( item->data() );
	}
	else if( !item->upperText().isEmpty() )
	{
		evt->ignore();
		return;
	}

	setAt(ui.devilList->row(item), devil);

	evt->acceptProposedAction();
	selectionChanged();
	updateCount();
	markDirty();
}

void COMP::clearAt(int index)
{
	IconListWidgetItem *item = ui.devilList->itemAt(index);
	if(!item)
		return;

	item->setIcon( QIcon(":/blank.png") );
	item->setUpperText(QString());
	item->setLowerText(QString());
	item->setData(QVariant());

	markDirty();
	updateCount();
}

void COMP::setAt(int index, const QVariantMap& devil)
{
	IconListWidgetItem *item = ui.devilList->itemAt(index);
	if(!item)
		return;

	DevilCache *cache = DevilCache::getSingletonPtr();

	QString tooltip = cache->devilToolTip(devil);

	QVariantMap devil_data = cache->devilByID( devil.value("id").toInt() );

	QIcon icon( QString("icons/devils/icon_%1.png").arg(
		devil_data.value("icon").toString() ) );

	item->setIcon(icon);
	item->setUpperText( devil_data.value("name").toString() );
	item->setData(devil);
	item->setToolTip(tooltip);

	markDirty();
	updateCount();
}

void COMP::updateCount()
{
	int count = 0;
	int max = ui.devilList->count();

	for(int i = 0; i < max; i++)
	{
		if( !ui.devilList->itemAt(i)->upperText().isEmpty() )
			count++;
	}

	ui.fuseButton->setEnabled(count > 0);
	ui.devilCount->setText(tr("%1/%2").arg(count).arg(max));
}

void COMP::dismiss()
{
	QList<IconListWidgetItem*> items = ui.devilList->selectedItems();
	if( !items.count() )
		return;

	// TODO: Ask if you are sure

	clearAt( ui.devilList->row(items.first()) );
	selectionChanged();
	updateCount();
}

void COMP::markDirty()
{
	if(!mLoaded || mMarked)
		return;

	mMarked = true;

	mSyncTimer->start(60000); // 60 seconds

	Taskbar::getSingletonPtr()->notifyDirty("simulator_sync_comp");
}

void COMP::sync()
{
	QVariantMap action;
	action["action"] = "simulator_sync_comp";
	action["user_data"] = "simulator_sync_comp";

	int max = ui.devilList->count();

	QVariantList devils;
	for(int i = 0; i < max; i++)
		devils << json::toJSON( ui.devilList->itemAt(i)->data() );

	action["devils"] = devils;

	ajax::getSingletonPtr()->request(settings->url(), action);

	// Stop the timer now
	mSyncTimer->stop();
	mMarked = false;
}

void COMP::loadDevils()
{
	QVariantMap action;
	action["action"] = "simulator_load_comp";
	action["user_data"] = "simulator_load_comp";

	ajax::getSingletonPtr()->request(settings->url(), action);
}

void COMP::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();

	if(result.value("user_data").toString() != "simulator_load_comp")
		return;

	int max = ui.devilList->count();

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

void COMP::itemDoubleClicked(IconListWidgetItem *item)
{
	if( item->data().toMap().isEmpty() )
	{
		// Release the mouse so the AddDevil can be used
		QMouseEvent release_event(QEvent::MouseMove, QPoint(-1, -1),
			QPoint(-1, -1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
		qApp->sendEvent(item, &release_event);

		// Empty slot, so add a devil
		mAddDevil->add( ui.devilList->row(item) );
	}
	else
	{
		DevilProperties *prop = DevilProperties::getSingletonPtr();

		prop->setActiveDevil( this,
			ui.devilList->row(item), item->data().toMap() );
		prop->show();
		prop->activateWindow();
		prop->raise();
	}
}

void COMP::properties()
{
	QList<IconListWidgetItem*> items = ui.devilList->selectedItems();
	if( items.isEmpty() )
		return;

	IconListWidgetItem *item = items.first();

	DevilProperties *prop = DevilProperties::getSingletonPtr();

	prop->setActiveDevil( this,
		ui.devilList->row(item), item->data().toMap() );
	prop->show();
	prop->activateWindow();
	prop->raise();
}
