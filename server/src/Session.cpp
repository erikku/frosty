/******************************************************************************\
*  server/src/Session.cpp                                                      *
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

#include "Session.h"
#include "Utils.h"

#include <QtCore/QUrl>
#include <QtCore/QRegExp>

static Session *g_session_inst = 0;

Session::Session(QObject *parent) : QObject(parent)
{
	mTimer = new QTimer;
	mTimer->start(300000);

	connect(mTimer, SIGNAL(timeout()), this, SLOT(clean()));
};

Session* Session::getSingletonPtr()
{
	if(!g_session_inst)
		g_session_inst = new Session;

	Q_ASSERT(g_session_inst);

	return g_session_inst;
};

QString Session::create(const QHostAddress& peer)
{
	QString hash = sha1FromString( QString::number(
		QDateTime::currentDateTime().toTime_t() ) + peer.toString() );

	QPair<QDateTime, QVariant> pair;
	pair.first = QDateTime::currentDateTime();
	pair.second = QVariant("nil");

	mSessions[hash] = pair;

	return hash;
};

QVariant Session::load(const QString& key)
{
	if( !mSessions.contains(key) )
		return QVariant(QVariant::Invalid);

	QPair<QDateTime, QVariant> pair = mSessions.value(key);

	QDateTime expires_at = pair.first.addSecs(300);

	if(QDateTime::currentDateTime() > expires_at) // The session has expired
	{
		mSessions.remove(key);

		return QVariant(QVariant::Invalid);
	}

	return pair.second;
};

void Session::save(const QString& key, QVariant& data)
{
	if( !mSessions.contains(key) )
		return;

	QPair<QDateTime, QVariant> pair;
	pair.first = QDateTime::currentDateTime();
	pair.second = data;

	mSessions[key] = pair;
};

QString Session::keyToCookie(const QString& key, const QString& host)
{
	// Strip port or whatever it leaves in there
	QUrl url( QString("http://%1/register.php").arg(host) );
	QString domain = url.host();

	return QString("SESSION=%1; domain=%2; path=/").arg(key).arg(domain);
};

QString Session::cookieToKey(const QString& cookie)
{
	QRegExp cookieMatcher("SESSION\\=([^\\;]+).*");
	if( cookieMatcher.exactMatch(cookie) )
		return cookieMatcher.cap(1).trimmed();

	return QString();
};

void Session::clean()
{
	QHash< QString, QPair<QDateTime, QVariant> >::iterator i;
	i = mSessions.begin();

	while( i != mSessions.end() )
	{
		QPair<QDateTime, QVariant> pair = i.value();
		QDateTime expires_at = pair.first.addSecs(300);

		// The session has expired, so remove the data
		if(QDateTime::currentDateTime() > expires_at)
			i = mSessions.erase(i);
		else
			i++;
	}
};
