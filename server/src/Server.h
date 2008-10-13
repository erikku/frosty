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

#include <QtCore/QCoreApplication>
#include <QtCore/QVariant>
#include <QtCore/QMap>

#include <QtNetwork/QHttpResponseHeader>
#include <QtNetwork/QTcpServer>
#include <QtSql/QSqlDatabase>

class Backend;
class QTcpSocket;

class SslServer : public QTcpServer
{
	Q_OBJECT

signals:
	void newConnection(QTcpSocket *socket);

protected slots:
	void error(QAbstractSocket::SocketError err);
	void sslErrors(const QList<QSslError>& errors);

protected:
	virtual void incomingConnection(int socketDescriptor);
};

class ConnectionData
{
public:
	QString header;
	QByteArray content;
	int contentLength;
	bool contentRead;
	bool done;
};

class Server : public QCoreApplication
{
	Q_OBJECT

public:
	Server(int argc, char *argv[]);

	void init();

protected slots:
	void readyRead();
	void handleNewConnection(QTcpSocket *socket);

protected:
	void read(QTcpSocket *connection);
	void error(QTcpSocket *connection);
	void finalize(QTcpSocket *connection);
	void respond(QTcpSocket *connection, const QVariant& data);
	void captcha(QTcpSocket *connection, const QString& session_key,
		const QString& cookie, const QString& error = QString());

	QHttpResponseHeader basicResponseHeader() const;

	QSqlDatabase db;
	Backend *mBackend;
	SslServer *mConnection;
	QMap<QTcpSocket*, ConnectionData*> mConnections;
};

#endif // __Server_h__
