/******************************************************************************\
*  server/src/TrayIcon.h                                                       *
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

#ifndef __TrayIcon_h__
#define __TrayIcon_h__

#include <QtGui/QSystemTrayIcon>

class TrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

public:
	TrayIcon(const QIcon& icon, QObject *parent = 0);

public slots:
	void shutdown();
	void showAbout();
	void showSettings();

protected:
};

#endif // __TrayIcon_h__
