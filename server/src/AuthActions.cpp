/******************************************************************************\
*  server/src/AuthActions.cpp                                                  *
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

#include "AuthActions.h"
#include "Auth.h"

QVariantMap authActionQueryPerms(int i, QIODevice *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(connection);

	if( action.contains("email") )
	{
		QVariantMap perms = auth->queryPerms(
			action.value("email").toString() ).toMap();
		perms.remove("any");

		QVariantMap map;
		map["email"] = action.value("email");
		map["perms"] = perms;

		return map;
	}
	else
	{
		QVariantMap perms = auth->queryPerms(email).toMap();
		perms.remove("any");

		QVariantMap map;
		map["email"] = email;
		map["perms"] = perms;

		return map;
	}
}

QVariantMap authActionQueryUsers(int i, QIODevice *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(action);
	Q_UNUSED(connection);

	QVariantMap map;
	map["users"] = auth->queryUsers(email);

	return map;
}

QVariantMap authActionQueryUser(int i, QIODevice *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(connection);

	QString target_email = action.value("email").toString();

	QVariantMap map;
	map["user"] = auth->queryUser(email, target_email);

	return map;
}

QVariantMap authActionMakeInactive(int i, QIODevice *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(connection);

	QString target_email = action.value("email").toString();
	auth->makeInactive(email, target_email);

	return QVariantMap();
}

QVariantMap authActionMakeActive(int i, QIODevice *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(connection);

	QString target_email = action.value("email").toString();
	auth->makeActive(email, target_email);

	return QVariantMap();
}

QVariantMap authActionModifyUser(int i, QIODevice *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(connection);

	QString target_email = action.value("email").toString();
	QString new_email = action.value("new_email").toString();
	QString name = action.value("name").toString();
	QString pass = action.value("pass").toString();
	QVariantMap perms = action.value("perms").toMap();

	auth->modifyUser(email, target_email, new_email, name, pass, perms);

	return QVariantMap();
}
