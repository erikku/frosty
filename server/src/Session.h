/******************************************************************************\
*  server/src/Session.h                                                        *
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

#ifndef __Session_h__
#define __Session_h__

#include <QtCore/QHash>
#include <QtCore/QPair>
#include <QtCore/QTimer>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtNetwork/QHostAddress>

class Session : public QObject
{
	Q_OBJECT

public:
	static Session* getSingletonPtr();

	// Creates a new session and returns a session key based on the current
	// time and the peer address.
	QString create(const QHostAddress& peer);

	// Loads the session data for the given session key (if the
	// session has expired, a QVariant::Invalid QVariant is returned)
	QVariant load(const QString& key);

	// Saves the session data for the given session key
	void save(const QString& key, QVariant& data);

	// Converts the session key to a cookie
	static QString keyToCookie(const QString& key, const QString& host);

	// Converts the cookie to a session key
	static QString cookieToKey(const QString& cookie);

public slots:
	void clean();

protected:
	Session(QObject *parent = 0);

	QTimer *mTimer;

	// Sessions are stored by session key and contain the last time they
	// were used and the session data stored as a QVariant.
	QHash< QString, QPair<QDateTime, QVariant> > mSessions;
};

#endif // __Session_h__
