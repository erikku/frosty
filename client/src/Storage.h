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
#include "StorageBase.h"

class QLabel;

class Storage : public StorageBase
{
	Q_OBJECT

public:
	Storage(QWidget *parent = 0);

	virtual int capacity() const;

	virtual DevilData devilAt(int index) const;
	virtual int activeIndex() const;

public slots:
	virtual void updateCount();

	virtual void clearAt(int index);
	virtual void setAt(int index, const DevilData& devil);
	virtual void setActiveIndex(int index);

protected:
	virtual int indexAt(const QPoint& pos) const;
	virtual QPoint indexPosition(int index) const;

	virtual QString loadUserData() const;
	virtual QVariantList loadData() const;

	virtual QString syncUserData() const;
	virtual QVariantList syncData() const;

private:
	int mActiveIndex;

	QList<QLabel*> mSlots;
	QList<DevilData> mData;

	Ui::Storage ui;
};

#endif // __Storage_h__
