/******************************************************************************\
*  client/src/COMP.h                                                           *
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

#ifndef __COMP_h__
#define __COMP_h__

#include "ui_COMP.h"

#include <QtCore/QPoint>

class Storage;
class IconListWidgetItem;

class COMP : public QWidget
{
	Q_OBJECT

	friend class Storage;

public:
	COMP(QWidget *parent = 0);

protected slots:
	void dismiss();
	void selectionChanged();

protected:
	void clearAt(int index);
	void setAt(int index, const QVariantMap& devil);

	void updateCount();

	IconListWidgetItem* itemAt(const QPoint& pos);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);

	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dragMoveEvent(QDragMoveEvent *event);
	virtual void dropEvent(QDropEvent *event);

	QPoint mDragStartPosition;

	Ui::COMP ui;
};

#endif // __COMP_h__
