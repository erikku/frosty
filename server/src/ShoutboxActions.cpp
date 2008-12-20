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
#include "Auth.h"

#include <QtCore/QQueue>
#include <QtCore/QDateTime>

QQueue<QVariantMap> g_shoutbox_queue;

QVariantMap shoutboxLogin(int i, QTcpSocket *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(email);
	Q_UNUSED(action);
	Q_UNUSED(connection);

	QVariantMap user_info = Auth::getSingletonPtr()->queryUser(
		email, email).toMap();

	QVariantMap ret = shoutboxPoll(i, connection, db, action, QString());
	ret["nick"] = user_info.value("name");

	return ret;
}

QVariantMap shoutboxPoll(int i, QTcpSocket *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
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

QVariantMap shoutboxPost(int i, QTcpSocket *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(connection);

	QVariantMap user_info = Auth::getSingletonPtr()->queryUser(
		email, email).toMap();

	QVariantMap msg;
	msg["author"] = user_info.value("name");
	msg["text"] = action.value("text");
	msg["email"] = email;
	msg["timestamp"] = QString::number(
		QDateTime::currentDateTime().toTime_t() );

	g_shoutbox_queue << msg;
	while(g_shoutbox_queue.count() > 20)
		g_shoutbox_queue.dequeue();

	return QVariantMap();
}
