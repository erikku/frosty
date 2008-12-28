/******************************************************************************\
*  client/src/Storage.h                                                        *
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

#ifndef __Storage_h__
#define __Storage_h__

#include "ui_Storage.h"

#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QtCore/QVariantMap>

class COMP;
class AddDevil;
class QLabel;
class QTimer;

class Storage : public QWidget
{
	Q_OBJECT

	friend class COMP;

public:
	Storage(QWidget *parent = 0);

public slots:
	void sync();

protected slots:
	void markDirty();
	void loadDevils();
	void ajaxResponse(const QVariant& resp);
	void setAt(int index, const QVariantMap& devil);

protected:
	void updateCount();
	void clearAt(int index);

	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);

	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dragMoveEvent(QDragMoveEvent *event);
	virtual void dropEvent(QDropEvent *event);

	QList<QLabel*> mSlots;
	QList<QVariantMap> mData;

	QPoint mDragStartPosition;

	bool mLoaded, mMarked;
	QTimer *mSyncTimer;

	AddDevil *mAddDevil;

	Ui::Storage ui;
};

#endif // __Storage_h__
