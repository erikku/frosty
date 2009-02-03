/******************************************************************************\
*  client/src/MoonPhases.h                                                     *
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

#ifndef __MoonPhases_h__
#define __MoonPhases_h__

#include "ui_MoonPhases.h"

#include <QtCore/QDateTime>

class MoonPhases : public QWidget
{
	Q_OBJECT

public:
	MoonPhases(QWidget *parent = 0);

	void addMoonPhase(const QDateTime& start,
		const QDateTime& stop, uint moon, const QString& phase);

	void clear();

public slots:
	void updateList();

protected:
	Ui::MoonPhases ui;
};

#endif // __MoonPhases_h__
