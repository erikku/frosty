/******************************************************************************\
*  client/src/DevilProperties.h                                                *
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

#ifndef __DevilProperties_h__
#define __DevilProperties_h__

#include "ui_DevilProperties.h"
#include "StorageBase.h"

class QLabel;
class QTimer;

class DevilProperties : public QWidget
{
	Q_OBJECT

public:
	static DevilProperties* getSingletonPtr();

	void setActiveDevil(StorageBase *storage, int slot, const DevilData& data);

protected slots:
	void updateLearnedSkills();

protected:
	DevilProperties(QWidget *parent = 0);

	void darkenWidget(QWidget *widget);

	void rewindSkills(int start, QList<QLabel*> *icons, QList<int> *ids);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);

	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dragMoveEvent(QDragMoveEvent *event);
	virtual void dropEvent(QDropEvent *event);

	int mActiveSlot;
	StorageBase *mActiveStorage;

	DevilData mData;

	QList<int> mActiveIDs, mLearnedIDs, mInheritedIDs;
	QList<QLabel*> mActiveSkills, mLearnedSkills, mInheritedSkills;

	QPoint mDragStartPosition;

	Ui::DevilProperties ui;
};

#endif // __DevilProperties_h__
