/******************************************************************************\
*  server/src/Server.h                                                         *
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

#ifndef __Server_h__
#define __Server_h__

#ifdef QT_GUI_LIB
#include <QtGui/QApplication>
#else // QT_GUI_LIB
#include <QtCore/QCoreApplication>
#endif // QT_GUI_LIB

#include <QtCore/QVariant>
#include <QtCore/QMap>

#include <QtSql/QSqlDatabase>

class QHttpResponseHeader;

class Backend;
class QIODevice;
class QLocalServer;
class QTcpSocket;
class SslServer;

class ConnectionData
{
public:
	QString header;
	QByteArray content;
	int contentLength;
	bool contentRead;
	bool done;
};

#ifdef QT_GUI_LIB
class Server : public QApplication
#else // QT_GUI_LIB
class Server : public QCoreApplication
#endif // QT_GUI_LIB
{
	Q_OBJECT

public:
	Server(int argc, char *argv[]);

	void init();

protected slots:
	void readyRead();
	void handleNewConnection(QTcpSocket *socket);
	void handleNewLocalConnection();

protected:
	void read(QIODevice *connection);
	void error(QIODevice *connection);
	void finalize(QIODevice *connection);
	void respond(QIODevice *connection, const QVariant& data);
	void captcha(QIODevice *connection, const QString& session_key,
		const QString& cookie, const QString& error = QString());

	inline QString createSessionKey(QIODevice *connection);

	QHttpResponseHeader basicResponseHeader() const;

	QSqlDatabase db;
	Backend *mBackend;
	SslServer *mSslConnection;
	QLocalServer *mLocalConnection;
	QMap<QIODevice*, ConnectionData*> mConnections;
};

#endif // __Server_h__
