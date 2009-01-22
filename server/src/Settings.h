/******************************************************************************\
*  server/src/Settings.h                                                       *
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

#ifndef __Settings_h__
#define __Settings_h__

#include "ui_Settings.h"

class Settings : public QWidget
{
	Q_OBJECT

public:
	Settings(QWidget *parent = 0);

public slots:
	void load();
	void save();

	void browseLogPath();
	void browseKeyPath();
	void browseCertPath();
	void browseFontPath();
	void browseDatabasePath();
	void browseAuthDatabasePath();
	void browseShoutboxLogPath();
	void updateConnectionType();

protected:
	void darkenWidget(QWidget *widget);

	Ui::Settings ui;
};

#endif // __Settings_h__
