/******************************************************************************\
*  client/src/FusionChart.h                                                    *
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

#ifndef __FusionChart_h__
#define __FusionChart_h__

#include "ui_FusionChart.h"
#include "StorageBase.h"

#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QDialog>

class FusionChart : public QDialog
{
	Q_OBJECT

public:
	FusionChart(QWidget *parent = 0);

	void loadDevils(StorageBase *storage, const QList<DevilData>& devils);

public slots:
	void clear();

protected:
	void calculateFusion(int first, int second);
	bool checkLevelRange(int level, const QVariantMap& devil);

	QVariantMap fuseDevils(const QVariantMap& devilA,
		const QVariantMap& devilB, const QVariantMap& resultDevil);

	virtual void mouseReleaseEvent(QMouseEvent *event);

	Ui::FusionChart ui;

	StorageBase *mStorage;

	QList<int> mDevilSlots;
	QList<QLabel*> mDevilLabels;
	QList<QLabel*> mDevilIconGrid;
	QList<QLabel*> mDevilIcons, mDevilIcons2;

	QList<QVariantMap> mDevilData;
	QList<QVariantMap> mFusionData;
};

#endif // __FusionChart_h__
