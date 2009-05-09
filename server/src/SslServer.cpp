/******************************************************************************\
*  server/src/SslServer.cpp                                                    *
*  Copyright (C) 2009 John Eric Martin <john.eric.martin@gmail.com>            *
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

#include "SslServer.h"
#include "Config.h"
#include "Log.h"

#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslKey>

void SslServer::incomingConnection(int descriptor)
{
	if( conf->sslEnabled() )
	{
		QSslSocket *socket = new QSslSocket;

		connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(error(QAbstractSocket::SocketError)));
		connect(socket, SIGNAL(sslErrors(const QList<QSslError>&)),
			this, SLOT(sslErrors(const QList<QSslError>&)));

		socket->setLocalCertificate( conf->sslCert() );
		socket->setPrivateKey( conf->sslKey() );

		socket->setSocketDescriptor(descriptor);
		socket->startServerEncryption();

		emit newConnection(socket);
	}
	else
	{
		QTcpSocket *socket = new QTcpSocket;

		connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(error(QAbstractSocket::SocketError)));

		socket->setSocketDescriptor(descriptor);

		emit newConnection(socket);
	}
}

void SslServer::error(QAbstractSocket::SocketError err)
{
	Q_UNUSED(err);

	QTcpSocket *socket = qobject_cast<QTcpSocket*>( sender() );
	Q_ASSERT(socket);

	LOG_ERROR( QString("Socket Error: %1\n").arg( socket->errorString() ) );
}

void SslServer::sslErrors(const QList<QSslError>& errors)
{
	foreach(QSslError err, errors)
		LOG_ERROR( QString("SSL Error: %1").arg( err.errorString() ) );
}
