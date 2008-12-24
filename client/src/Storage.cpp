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
#include "COMP.h"

#include <QtCore/QMimeData>
#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>

Storage::Storage(QWidget *parent_widget) : QWidget(parent_widget)
{
	ui.setupUi(this);

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

	QString icon_path = QString("icons/devils/icon_%1.png").arg("D2_04_009a");
	QPixmap icon(icon_path);

	QVariantMap devil;
	devil["name_en"] = "Jack Frost";
	devil["name_ja"] = QString::fromUtf8("ジャックフロスト");
	devil["icon"] = "D2_04_009a";

	foreach(QLabel *slot, mSlots)
	{
		slot->setPixmap(icon);
		mData << devil;
	}

	devil["name_en"] = "Oni";
	devil["name_ja"] = QString::fromUtf8("オニ");
	devil["icon"] = "D3_02_011a";

	icon_path = QString("icons/devils/icon_%1.png").arg("D3_02_011a");
	icon = QPixmap(icon_path);

	mSlots.at(5)->setPixmap(icon);
	mData[5] = devil;

	setAcceptDrops(true);
}

void Storage::mousePressEvent(QMouseEvent *evt)
{
	if(evt->buttons() & Qt::LeftButton)
		mDragStartPosition = evt->pos();
}

void Storage::mouseMoveEvent(QMouseEvent *evt)
{
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
}

void Storage::setAt(int index, const QVariantMap& devil)
{
	if(index < 0 || index >= mSlots.count())
		return;

	QString icon_path = QString("icons/devils/icon_%1.png").arg(
		devil.value("icon").toString() );

	mSlots.at(index)->setPixmap( QPixmap(icon_path) );
	mData[index] = devil;
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
