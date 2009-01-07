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
#include "StorageBase.h"

class COMP : public StorageBase
{
	Q_OBJECT

public:
	COMP(QWidget *parent = 0);

	virtual int capacity() const;

	virtual DevilData devilAt(int index) const;
	virtual int activeIndex() const;

public slots:
	virtual void updateCount();

	virtual void clearAt(int index);
	virtual void setAt(int index, const DevilData& devil);
	virtual void setActiveIndex(int index);

protected slots:
	void itemDoubleClicked(IconListWidgetItem *item);

protected:
	virtual int indexAt(const QPoint& pos) const;
	virtual QPoint indexPosition(int index) const;

	virtual QString loadUserData() const;
	virtual QVariantList loadData() const;

	virtual QString syncUserData() const;
	virtual QVariantList syncData() const;

	virtual QPoint hotspot(int index) const;

	virtual void mouseMoveEvent(QMouseEvent *event);

private:
	Ui::COMP ui;
};

#endif // __COMP_h__
