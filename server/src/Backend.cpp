/******************************************************************************\
*  server/src/Backend.cpp                                                      *
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

#include "Backend.h"
#include "Utils.h"
#include "Auth.h"
#include "Log.h"

#include "AuthActions.h"
#include "BackendActions.h"
#include "DatabaseActions.h"
#include "ServerActions.h"

#include "json.h"

Backend::Backend(QObject *parent) : QObject(parent)
{
	// Standard Actions
	mActionHandlers["salt"] = backendActionSalt;
	mActionHandlers["delete"] = backendActionDelete;
	mActionHandlers["insert"] = backendActionInsert;
	mActionHandlers["update"] = backendActionUpdate;
	mActionHandlers["select"] = backendActionSelect;

	// Database Actions
	mActionHandlers["db_export"] = dbActionExport;
	mActionHandlers["db_import"] = dbActionImport;

	// Auth Actions
	mActionHandlers["auth_query_perms"] = authActionQueryPerms;
	mActionHandlers["auth_query_users"] = authActionQueryUsers;
	mActionHandlers["auth_query_user"] = authActionQueryUser;
	mActionHandlers["auth_make_inactive"] = authActionMakeInactive;
	mActionHandlers["auth_make_active"] = authActionMakeActive;
	mActionHandlers["auth_modify_user"] = authActionModifyUser;

	// Server Actions
	mActionHandlers["server_updates"] = serverActionUpdates;

	auth->start(mActionHandlers);
};

QVariantList Backend::parseRequest(QTcpSocket *connection,
	const QSqlDatabase& db, const QMap<QString, QString>& post) const
{
	QVariant request = json::parse( post.value("request") );
	QVariantList request_results;

	if( !request.canConvert(QVariant::Map) )
	{
		request_results << herror(tr("request"),
			tr("Request is not an object") );

		return request_results;
	}
	else if( !request.toMap().contains("actions") )
	{
		request_results << herror(tr("request"),
			tr("Request does not contain any actions") );

		return request_results;
	}

	QString email;
	if( post.contains("email") )
	{
		email = post.value("email");

		if(  !auth->authenticate(email, post.value("request"),
			post.value("pass")) )
		{
			QVariantMap error;
			error["error"] = "Authentication Error";

			QVariantList list;
			list << error;

			return list;
		}
	}

	QVariantList actions = request.toMap().value("actions").toList();

	for(int i = 0; i < actions.count(); i++)
	{
		QVariant var_action = actions.at(i);

		if( !var_action.canConvert(QVariant::Map) )
		{
			request_results << herror(tr("action"),
				tr("action %1 is not an object").arg(i) );

			continue;
		}

		QVariantMap action = var_action.toMap();

		if( !action.contains("action") )
		{
			request_results << herror(tr("action"),
				tr("action %1 has no type").arg(i) );

			continue;
		}

		if( !action.value("action").canConvert(QVariant::String) )
		{
			request_results << herror(tr("action"),
				tr("type for action %1 is not a string").arg(i) );

			continue;
		}

		QString name = action.value("action").toString();

		if( mActionHandlers.contains(name) )
		{
			QVariantMap request_result;
			if( auth->validateRequest(email, action) )
			{
				request_result = mActionHandlers.value(name)(i, connection, db,
					action, email);
			}
			else
			{
				request_result = herror(tr("%1 action").arg(name),
					tr("authentication error for action %1").arg(i));
			}

			if( action.contains("user_data") )
				request_result["user_data"] = action.value("user_data");

			request_result["action"] = name;

			request_results << request_result;
		}
		else
		{
			request_results << herror(tr("action"),
				tr("unknown action type %1 for action %2").arg(name).arg(i) );

			continue;
		}
	}

	return request_results;
};
