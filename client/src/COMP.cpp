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

#include "json.h"
#include "DevilCache.h"
#include "IconListWidgetItem.h"

#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>

COMP::COMP(QWidget *parent_widget) : StorageBase(parent_widget)
{
	ui.setupUi(this);
	setEditable(true);

	setWindowTitle( tr("%1 - COMP").arg(
		tr("Absolutely Frosty") ) );

	connect(ui.devilList, SIGNAL(selectionChanged()),
		this, SLOT(selectionChanged()));
	connect(ui.devilList, SIGNAL(itemDoubleClicked(IconListWidgetItem*)),
		this, SLOT(itemDoubleClicked(IconListWidgetItem*)));

	QIcon blank(":/border.png");

	int max = capacity();

	// Fill in blanks
	for(int i = 0; i < max; i++)
	{
		IconListWidgetItem *item = new IconListWidgetItem(blank);
		ui.devilList->addItem(item);
	}

	initStorage(ui.addButton, ui.dismissButton, ui.propButton,
		ui.duplicateButton, ui.extractButton, ui.fuseButton);
}

int COMP::capacity() const
{
	return 8;
}

DevilData COMP::devilAt(int index) const
{
	Q_ASSERT(index < capacity() || index >= 0);
	if(index >= capacity() || index < 0)
		return DevilData();

	return ui.devilList->itemAt(index)->data().toMap();
}

int COMP::activeIndex() const
{
	QList<IconListWidgetItem*> items = ui.devilList->selectedItems();
	if( items.isEmpty() )
		return -1;

	return ui.devilList->row( items.first() );
}

void COMP::updateCount()
{
	int used = count();
	int max = capacity();

	ui.fuseButton->setEnabled(used > 0);
	ui.devilCount->setText(tr("%1/%2").arg(used).arg(max));
}

void COMP::clearAt(int index)
{
	Q_ASSERT(index < capacity() || index >= 0);
	if(index >= capacity() || index < 0)
		return;

	IconListWidgetItem *item = ui.devilList->itemAt(index);
	Q_ASSERT(item);

	item->setIcon( QIcon(":/border.png") );
	item->setUpperText(QString());
	item->setLowerText(QString());
	item->setToolTip(QString());
	item->setData(QVariant());

	markDirty();
	updateCount();
	selectionChanged();
}

void COMP::setAt(int index, const DevilData& devil)
{
	Q_ASSERT(index < capacity() || index >= 0);
	if(index >= capacity() || index < 0)
		return;

	IconListWidgetItem *item = ui.devilList->itemAt(index);
	Q_ASSERT(item);

	DevilCache *cache = DevilCache::getSingletonPtr();

	QString tooltip = cache->devilToolTip(devil);

	DevilData devil_data = cache->devilByID( devil.value("id").toInt() );

	QIcon icon( QString("icons/devils/icon_%1.png").arg(
		devil_data.value("icon").toString() ) );

	item->setIcon(icon);
	item->setUpperText( devil_data.value("name").toString() );
	item->setData(devil);
	item->setToolTip(tooltip);

	markDirty();
	updateCount();
	selectionChanged();
}

void COMP::setActiveIndex(int index)
{
	Q_ASSERT(index < capacity() || index >= 0);
	if(index >= capacity() || index < 0)
		return;

	ui.devilList->setCurrentRow(index);
}

void COMP::itemDoubleClicked(IconListWidgetItem *item)
{
	Q_ASSERT(ui.devilList->row(item) >= 0);

	// Release the mouse so the AddDevil can be used
	QMouseEvent release_event(QEvent::MouseMove, QPoint(-1, -1),
		QPoint(-1, -1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
	qApp->sendEvent(item, &release_event);

	indexDoubleClicked( ui.devilList->row(item) );
}

int COMP::indexAt(const QPoint& p) const
{
	QObject *child = childAt(p);
	if(!child)
		return -1;

	int index = -1;

	do
	{
		if( !child->inherits("IconListWidgetItem") )
			continue;

		index = ui.devilList->row( qobject_cast<IconListWidgetItem*>(child) );

		break;
	} while( (child = child->parent()) != this && child );

	return index;
}

QPoint COMP::indexPosition(int index) const
{
	Q_ASSERT(index < capacity() || index >= 0);
	if(index >= capacity() || index < 0)
		return QPoint(0, 0);

	IconListWidgetItem *item = ui.devilList->itemAt(index);
	Q_ASSERT(item);

	return item->pos();
}

QString COMP::loadUserData() const
{
	return "simulator_load_comp";
}

QVariantList COMP::loadData() const
{
	QVariantMap action;
	action["action"] = "simulator_load_comp";
	action["user_data"] = "simulator_load_comp";

	return QVariantList() << action;
}

QString COMP::syncUserData() const
{
	return "simulator_sync_comp";
}

QVariantList COMP::syncData() const
{
	QVariantMap action;
	action["action"] = "simulator_sync_comp";
	action["user_data"] = "simulator_sync_comp";

	int max = capacity();

	QVariantList devils;
	for(int i = 0; i < max; i++)
		devils << json::toJSON( ui.devilList->itemAt(i)->data() );

	action["devils"] = devils;

	return QVariantList() << action;
}

QPoint COMP::hotspot(int index) const
{
	Q_UNUSED(index);

	return QPoint(16, 16);
}

void COMP::mouseMoveEvent(QMouseEvent *evt)
{
	// If we are starting to drag an item, we need to release the mouse

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

	// Release the mouse from the item so the drag works right
	QMouseEvent release_event(QEvent::MouseMove, QPoint(-1, -1),
		QPoint(-1, -1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
	qApp->sendEvent(ui.devilList->itemAt(index), &release_event);

	StorageBase::mouseMoveEvent(evt);
}
