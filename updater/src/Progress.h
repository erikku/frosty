/******************************************************************************\
*  updater/src/Progress.h                                                      *
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

#ifndef __Progress_h__
#define __Progress_h__

#include "ui_Progress.h"

class Progress : public QWidget
{
	Q_OBJECT

public:
	Progress(QWidget *parent = 0);

public slots:
	void setMaxFiles(int max);
	void setProgress(const QString& file, int value);

protected:
	Ui::Progress ui;
};

#endif // __Progress_h__
