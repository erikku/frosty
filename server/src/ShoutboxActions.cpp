/******************************************************************************\
*  server/src/ShoutboxActions.cpp                                              *
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

#include "ShoutboxActions.h"
#include "Config.h"
#include "Auth.h"
#include "Log.h"

#include <QtCore/QFile>
#include <QtCore/QQueue>
#include <QtCore/QDateTime>

QQueue<QVariantMap> g_shoutbox_queue;

QVariantMap shoutboxLogin(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(email);
	Q_UNUSED(action);
	Q_UNUSED(user_db);
	Q_UNUSED(connection);

	QVariantMap user_info = Auth::getSingletonPtr()->queryUser(
		email, email).toMap();

	QVariantMap perms = Auth::getSingletonPtr()->queryPerms(email).toMap();

	QVariantMap ret = shoutboxPoll(i, connection,
		db, user_db, action, QString());
	ret["modify_db"] = perms.value("modify_db");
	ret["nick"] = user_info.value("name");
	ret["admin"] = perms.value("admin");

	return ret;
}

QVariantMap shoutboxPoll(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(user_db);
	Q_UNUSED(connection);

	uint stamp = action.value("timestamp").toUInt();

	QVariantList msgs;
	foreach(QVariantMap msg, g_shoutbox_queue)
	{
		if(msg.value("email").toString() == email)
			continue;

		if(msg.value("timestamp").toUInt() > stamp)
		{
			QVariantMap copy = msg;
			copy.remove("email");

			msgs << copy;
		}
	}

	QVariantMap map;
	map["messages"] = msgs;
	map["timestamp"] = QString::number(
		QDateTime::currentDateTime().toTime_t() );

	return map;
}

QVariantMap shoutboxPost(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(user_db);
	Q_UNUSED(connection);

	QVariantMap user_info = Auth::getSingletonPtr()->queryUser(
		email, email).toMap();

	QVariantMap perms = Auth::getSingletonPtr()->queryPerms(email).toMap();

	QString text = action.value("text").toString();
	QString nick = user_info.value("name").toString();

	QVariantMap msg;
	msg["author"] = nick;
	msg["text"] = text;
	msg["email"] = email;
	msg["admin"] = perms.value("admin");
	msg["modify_db"] = perms.value("modify_db");
	msg["timestamp"] = QString::number(
		QDateTime::currentDateTime().toTime_t() );

	if( conf->shoutboxLog() )
	{
		QString stamp = QDateTime::currentDateTime().toString(
			"yyyy-MM-dd hh:mm:ss");

		QFile log( conf->shoutboxLogPath() );
		if( !log.open(QIODevice::WriteOnly | QIODevice::Append) )
		{
			LOG_ERROR("Failed to open shoutbox log for writing.\n");
		}
		else
		{
			QString line = QString("<b>(%1) %2:</b> %3<br/>\n").arg(
				stamp).arg(nick).arg(text);

			log.write( line.toUtf8() );
			log.flush();
			log.close();
		}
	}

	g_shoutbox_queue << msg;
	while(g_shoutbox_queue.count() > 20)
		g_shoutbox_queue.dequeue();

	return QVariantMap();
}
