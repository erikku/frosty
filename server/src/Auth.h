/******************************************************************************\
*  server/src/Auth.h                                                           *
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

#ifndef __Auth_h__
#define __Auth_h__

#include "Backend.h"

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtSql/QSqlDatabase>

class Auth
{
public:
	static Auth* getSingletonPtr();

	void start(const QMap<QString, BackendActionHandler>& actionHandlers);
	bool authenticate(const QString& email, const QString& data,
		const QString& hash) const;
	bool validateRequest(const QString& email, const QVariant& action) const;

	QVariant queryPerms(const QString& email) const;
	QVariant queryUsers(const QString& email) const;
	QVariant queryUser(const QString& email, const QString& target) const;

	int queryUserID(const QString& email) const;

	QString registerUser(const QString& email, const QString& name,
		const QString& pass) const;

	bool makeInactive(const QString& email, const QString& target_email) const;
	bool makeActive(const QString& email, const QString& target_email) const;

	bool modifyUser(const QString& email, const QString& target_email,
		const QString& new_email, const QString& name, const QString& pass,
		const QVariantMap& perms) const;

protected:
	Auth();

	QSqlDatabase mAuthDB;
	QVariantMap mDefaultPerms;
	QMap<QString, QString> mActionPermissions;
};

#define auth ( Auth::getSingletonPtr() )

#endif // __Auth_h__
