/******************************************************************************\
*  client/src/Clock.h                                                          *
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

#ifndef __Clock_h__
#define __Clock_h__

#include "ui_Clock.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QDateTime>

class Clock : public QWidget
{
	Q_OBJECT

public:
	Clock(QWidget *parent = 0, Qt::WindowFlags f = 0);

public slots:
	void updateClocks();

protected:
	void darkenWidget(QWidget *widget);

	void megatenTime(const QDateTime& stamp, uint now);

	Ui::Clock ui;

	QList<QString> days, phases;
};

#endif // __Clock_h__
