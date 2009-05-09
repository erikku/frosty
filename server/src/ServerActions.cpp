/******************************************************************************\
*  server/src/ServerActions.cpp                                                *
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

#include "ServerActions.h"
#include "Config.h"

QVariantMap serverActionUpdates(int i, QIODevice *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(email);
	Q_UNUSED(action);
	Q_UNUSED(connection);

	QVariantMap map;
	map["client_win32"]   = conf->clientWin32();
	map["client_macosx"]  = conf->clientMacOSX();
	map["client_linux"]   = conf->clientLinux();
	map["updater_win32"]  = conf->updaterWin32();
	map["updater_macosx"] = conf->updaterMacOSX();
	map["updater_linux"]  = conf->updaterLinux();

	return map;
}
