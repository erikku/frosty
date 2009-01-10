/******************************************************************************\
*  client/src/StorageBase.h                                                    *
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

#ifndef __StorageBase_h__
#define __StorageBase_h__

#include <QtCore/QVariantMap>
#include <QtCore/QPoint>
#include <QtCore/QList>

#include <QtGui/QWidget>

typedef QVariantMap DevilData;

class QTimer;
class QPushButton;

class AddDevil;
class FusionChart;

class StorageBase : public QWidget
{
	Q_OBJECT

public:
	StorageBase(QWidget *parent = 0);

	bool isEditable() const;

	int count() const;

	virtual int capacity() const = 0;

	virtual DevilData devilAt(int index) const = 0;
	virtual int activeIndex() const = 0;

signals:
	void devilDataChanged();

public slots:
	void add();
	void dismiss();
	void extract();
	void duplicate();
	void properties();
	void startFusion();

	void sync();
	void markDirty();
	void loadDevils();

	virtual void updateCount() = 0;

	virtual void clearAt(int index) = 0;
	virtual void setAt(int index, const DevilData& devil) = 0;
	virtual void setActiveIndex(int index) = 0;

protected slots:
	void selectionChanged();
	void indexDoubleClicked(int index = -1);
	void ajaxResponse(const QVariantMap& resp, const QString& user_data);

protected:
	void initStorage(QPushButton *addButton, QPushButton *dismissButton,
		QPushButton *propertiesButton, QPushButton *duplicateButton = 0,
		QPushButton *extractButton = 0, QPushButton *fuseButton = 0);

	int nextFreeIndex() const;

	QList<DevilData> readLoadData(const QVariantMap& result) const;

	void setEditable(bool editable);

	virtual int indexAt(const QPoint& pos) const = 0;
	virtual QPoint indexPosition(int index) const = 0;

	virtual QString loadUserData() const = 0;
	virtual QVariantList loadData() const = 0;

	virtual QString syncUserData() const = 0;
	virtual QVariantList syncData() const = 0;

	virtual QPoint hotspot(int index) const;

	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);

	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dragMoveEvent(QDragMoveEvent *event);
	virtual void dropEvent(QDropEvent *event);

	virtual void contextMenuEvent(QContextMenuEvent *event);

protected:
	QPoint mDragStartPosition;

private:
	bool mEditable;
	bool mLoaded, mMarked;

	QTimer *mSyncTimer;
	AddDevil *mAddDevil;
	FusionChart *mFusionChart;

	QPushButton *mFuseButton, *mPropertiesButton, *mDismissButton;
	QPushButton *mAddButton, *mDuplicateButton, *mExtractButton;
};

#endif // __StorageBase_h__
