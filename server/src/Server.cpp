/******************************************************************************\
*  server/src/Server.cpp                                                       *
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

#include "Server.h"
#include "Session.h"
#include "Backend.h"
#include "Config.h"
#include "Utils.h"
#include "Auth.h"
#include "json.h"
#include "Log.h"

#ifdef QT_GUI_LIB
#include "TrayIcon.h"
#endif // QT_GUI_LIB

#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtNetwork/QHttpRequestHeader>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslKey>
#include <gd.h>

#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlDatabase>


#ifdef QT_GUI_LIB
Server::Server(int ac, char *av[]) : QApplication(ac, av)
{
	setWindowIcon( QIcon( ":/megatendb.png") );
	setQuitOnLastWindowClosed(false);

	( new TrayIcon( windowIcon() ) )->show();
#else // QT_GUI_LIB
Server::Server(int ac, char *av[]) : QCoreApplication(ac, av)
{
#endif // QT_GUI_LIB

	qsrand( QDateTime::currentDateTime().toTime_t() );
}

void Server::init()
{
	conf->loadConfig("config.xml");

	mBackend = new Backend;
	mConnection = new SslServer;

	connect(mConnection, SIGNAL(newConnection(QTcpSocket*)),
		this, SLOT(handleNewConnection(QTcpSocket*)));

	if(conf->address() == "any")
		mConnection->listen(QHostAddress::Any, conf->port());
	else if(conf->address() == "localhost")
		mConnection->listen(QHostAddress::LocalHost, conf->port());
	else
		mConnection->listen(QHostAddress(conf->address()), conf->port());

	if(conf->dbType() == "sqlite")
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "master");
		db.setDatabaseName( conf->dbPath() );
		if( !db.open() )
			LOG_ERROR( db.lastError().text() );
	}
	else // mysql
	{
		db = QSqlDatabase::addDatabase("QMYSQL", "master");
		db.setHostName( conf->dbHost() );
		db.setDatabaseName( conf->dbName() );
		db.setUserName( conf->dbUser() );
		db.setPassword( conf->dbPass() );
		db.open();

		QSqlQuery query("SET CHARSET utf8", db);
		query.exec();
	}

	//db.close();
	//QSqlDatabase::removeDatabase("master");
}

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

void Server::handleNewConnection(QTcpSocket *socket)
{
	Q_ASSERT(mConnection);

	QTcpSocket *connection = socket;

	connect(connection, SIGNAL(readyRead()),
		this, SLOT(readyRead()));
	connect(connection, SIGNAL(disconnected()),
		connection, SLOT(deleteLater()));

	connection->open(QIODevice::ReadWrite);

	ConnectionData *data = new ConnectionData;
	mConnections[connection] = data;
	data->contentRead = false;
	data->contentLength = 0;
	data->done = false;

	read(connection);
}

void Server::readyRead()
{
	QTcpSocket *connection = qobject_cast<QTcpSocket*>(sender());
	Q_ASSERT(connection);

	read(connection);
}

void Server::read(QTcpSocket *connection)
{
	ConnectionData *data = mConnections.value(connection);
	Q_ASSERT(data);

	if(data->done)
	{
		// Sink
		connection->readAll();

		return;
	}

	while( !data->contentRead && connection->canReadLine() )
	{
		QString line = QString::fromUtf8( connection->readLine() );
		data->header += line;

		if( line.trimmed().isEmpty() )
		{
			QHttpRequestHeader header(data->header);

			data->contentLength = header.contentLength();
			data->contentRead = true;

			break;
		}
	}

	if(!data->contentRead)
		return;

	data->content.append( connection->readAll() );
	if(data->content.length() >= data->contentLength)
		finalize(connection);
}

void Server::finalize(QTcpSocket *connection)
{
	ConnectionData *data = mConnections.value(connection);
	Q_ASSERT(data);

	data->done = true;

	// HTTP Header
	QHttpRequestHeader header(data->header);

	// HTTP Content
	QString content = QString::fromUtf8(data->content);

	QRegExp postMatcher("([^\\=]+)\\=(.*)");

	QMap<QString, QString> post;
	foreach(QString item, content.split('&'))
	{
		if( !postMatcher.exactMatch(item) )
			continue;

		QString key = postMatcher.cap(1).trimmed();
		QString value = QUrl::fromPercentEncoding(
			postMatcher.cap(2).trimmed().toUtf8().replace("+", "%20") );

		post[key] = value;
	}

	if(post.contains("request") && header.method() == "POST"
		&& header.path() == "/backend.php")
	{
		QVariantList set = mBackend->parseRequest(connection, db, post);
		QVariant response = set;

		respond(connection, response);
	}
	else if(header.method() == "GET" && header.path() == "/register.php")
	{
		// GET request, so just create a session and send over a CAPTCHA

		// Here is the session key we are using to store the hash
		QString session_key;
		if( header.hasKey("cookie") )
		{
			session_key = Session::cookieToKey(header.value("cookie"));

			if( !Session::getSingletonPtr()->load(session_key).isValid() )
			{
				// The session is expired/invalid and we need to
				// create a new session.
				session_key = Session::getSingletonPtr()->create(
					connection->peerAddress() );
			}
		}
		else
		{
			// There is no existing session so just create a new one.
			session_key = Session::getSingletonPtr()->create(
				connection->peerAddress() );
		}

		QString cookie = Session::keyToCookie(session_key,
			header.value("host") );

		captcha(connection, session_key, cookie);
	}
	else if(header.method() == "POST" && header.path() == "/register.php")
	{
		if( header.hasKey("cookie") )
		{
			Session *session = Session::getSingletonPtr();

			QString session_key = session->cookieToKey(header.value("cookie"));
			QVariant session_data = session->load(session_key);
			if( !session_data.isValid() )
			{
				// The session is no longer valid (expired?) so make a new one.
				session_key = Session::getSingletonPtr()->create(
					connection->peerAddress() );

				QString cookie = Session::keyToCookie(session_key,
					header.value("host") );

				captcha(connection, session_key, cookie,
					"Session Expired");

				return;
			}

			QString cookie = Session::keyToCookie(session_key,
				header.value("host") );

			if( !post.contains("pass") || !post.contains("email") ||
				!post.contains("code") || !post.contains("name") )
			{
				captcha(connection, session_key, cookie,
					"Missing proper POST variables");

				return;
			}

			QString proper_hash = session_data.toString();

			// Check the code
			QString hash = sha1FromString(conf->saltImg() + post.value("code"));

			if(proper_hash != hash)
			{
				captcha(connection, session_key, cookie, "Invalid code");

				return;
			}

			QString err = auth->registerUser(post.value("email"),
				post.value("name"), post.value("pass"));
			if( !err.isEmpty() )
			{
				captcha(connection, session_key, cookie, err);

				return;
			}

			QHttpResponseHeader resp_header = basicResponseHeader();
			resp_header.setContentLength(2);
			resp_header.setContentType("text/html");

			connection->write( resp_header.toString().toUtf8() );
			connection->write( QString("OK").toUtf8() );
		}
		else
		{
			QString session_key = Session::getSingletonPtr()->create(
				connection->peerAddress() );

			QString cookie = Session::keyToCookie(session_key,
				header.value("host") );

			captcha(connection, session_key, cookie, "Invalid session");
		}
	}
	else
	{
		error(connection);
	}
}

QHttpResponseHeader Server::basicResponseHeader() const
{
#if defined(Q_WS_WIN)
	QString powered_by = tr("Qt v%1 running on Windows").arg(qVersion());
#elif defined(Q_WS_MAC)
	QString powered_by = tr("Qt v%1 running on Mac OS X").arg(qVersion());
#else
	QString powered_by = tr("Qt v%1 running on Linux").arg(qVersion());
#endif

	QString signature = conf->signature();
	signature.replace("${version}", "0.1.0");

	QHttpResponseHeader header;
	header.setStatusLine(200, "OK");
	header.setValue("Server", signature);
	header.setValue("X-Powered-By", powered_by);

	return header;
}

void Server::error(QTcpSocket *connection)
{
	QFile file(":/400.html");
	file.open(QIODevice::ReadOnly);

	QString signature = conf->signature();
	signature.replace("${version}", "0.1.0");

	QString content = file.readAll();
	content.replace("${signature}", signature);

	file.close();

	QHttpResponseHeader header = basicResponseHeader();
	header.setStatusLine(400, "Bad Request");
	header.setContentLength( content.toUtf8().length() );
	header.setContentType("text/html; charset=UTF-8");

	connection->write( QString(header.toString() + content).toUtf8() );
	connection->close();
}

void Server::respond(QTcpSocket *connection, const QVariant& data)
{
	QHttpResponseHeader header = basicResponseHeader();
	header.setValue("X-JSON", QString("(%1)").arg( json::toJSON(data) ));
	header.setContentLength(0);
	header.setContentType("application/x-json");

	connection->write( header.toString().toUtf8() );
	connection->close();
}

void Server::captcha(QTcpSocket *connection, const QString& session_key,
	const QString& cookie, const QString& error_text)
{
	QStringList letters = conf->captchaLetters();
	int count = letters.count();

	QString captcha_font = QDir::current().filePath( conf->captchaFont() );

	QString text;
	for(int i = 0; i < 8; i++)
		text += letters.at(qrand() % count);

	// Generate the hash and save it to the session data
	QString hash = sha1FromString(conf->saltImg() + text);

	QVariant session_data = hash;
	Session::getSingletonPtr()->save(session_key, session_data);

	// Get the bounding box for the text
	int brect[8];
	char *err = gdImageStringFT(NULL, brect, 0, captcha_font.toUtf8().data(),
		20, 0, 0, 0, text.toUtf8().data());
	if(err)
		LOG_ERROR(err);

	int width = brect[2] - brect[6];
	int height = brect[3] - brect[7];

	gdImagePtr im = gdImageCreateTrueColor(width, height);

	int background = gdImageColorAllocateAlpha(im, 255, 255, 255, 127);
	int font_color = gdImageColorAllocate(im, 0, 192, 0);

	gdImageFilledRectangle(im, 0, 0, width - 1, height - 1, background);

	// Add the text
	err = gdImageStringFT(im, brect, font_color, captcha_font.toUtf8().data(),
		20, 0, -brect[6], -brect[7], text.toUtf8().data());
	if(err)
		LOG_ERROR(err);

	int size = 0;
	char *image = (char*)gdImagePngPtr(im, &size);

	QDateTime stamp = QDateTime::currentDateTime().toUTC();
	QString time_stamp = QString("%1 GMT").arg( stamp.toString(
		"ddd, dd MMM yyyy hh:mm:ss") );

	QString pass_salt = conf->saltPass();

	QHttpResponseHeader header = basicResponseHeader();
	header.setValue("Expires", "Mon, 23 Jul 1993 05:00:00 GMT");
	header.setValue("Last-Modified", time_stamp);
	header.setValue("Cache-Control", "no-store, no-cache, must-revalidate, "
		"post-check=0, pre-check=0");
	header.setValue("X-Registration-Salt", pass_salt);
	header.setValue("Pragma", "no-cache");
	header.setValue("Set-Cookie", cookie);
	header.setContentLength(size);
	header.setContentType("image/png");

	if( !error_text.isEmpty() )
		header.setValue("X-Registration-Error", error_text);

	connection->write( header.toString().toUtf8() );
	connection->write(image, size);
	connection->close();

	gdFree(image);
	gdImageDestroy(im);
}
