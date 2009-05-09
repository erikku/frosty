/******************************************************************************\
*  client/src/localTransfer.cpp                                                *
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

#include "localTransfer.h"
#include "ajax.h"
#include "json.h"

#include <QtCore/QTimer>
#include <QtCore/QStringList>
#include <QtGui/QMessageBox>

#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QHttpResponseHeader>

localTransfer::localTransfer(QObject *parent_object) :
	baseTransfer(parent_object)
{
	mHaveHeader = false;
	mContentLength = 0;
}

localTransfer::~localTransfer()
{
	if(mLocal)
	{
		delete mLocal;
		mLocal = 0;
	}
}

localTransfer* localTransfer::start(const QMap<QString, QString>& post)
{
	localTransfer *transfer = new localTransfer;

	transfer->mLocal = new QLocalSocket;

	QHttpRequestHeader header(post.isEmpty() ? "GET" : "POST", "/backend.php");

	QByteArray data;
	if( !post.isEmpty() )
	{
		QStringList post_vars;
		QMapIterator<QString, QString> i(post);
		while( i.hasNext() )
		{
			i.next();
			QString post_var = i.key();
			post_var += "=";
			post_var += QString( QUrl::toPercentEncoding( i.value() ) );

			post_vars << post_var;
		}

		data = post_vars.join("&").toAscii();

		header.setContentType("application/x-www-form-urlencoded; "
			"charset=UTF-8");
		header.setContentLength( data.length() );
	}

	header.addValue("User-Agent", "Qt4 JSON");
	header.addValue("Host", "local");

	connect(transfer->mLocal, SIGNAL(readyRead()), transfer, SLOT(readyRead()));

	transfer->mLocal->connectToServer("frosty_local");
	if( !transfer->mLocal->waitForConnected() )
	{
		transfer->error( transfer->mLocal->errorString() );

		return transfer;
	}

	transfer->mLocal->write(header.toString().toUtf8());
	transfer->mLocal->write(data);

	return transfer;
}

void localTransfer::error(const QString& error)
{
	mError = error;
	QTimer::singleShot(0, this, SLOT(dispatchError()));
}

void localTransfer::dispatchError()
{
	emit transferFailed(mError);
}

void localTransfer::requestFinished(bool error)
{
	emit transferInfo(QString::fromUtf8(mContent), mResponse);

	QString content_error = QString::fromUtf8(mContent);
	if( !content_error.isEmpty() )
	{
		emit transferFailed(content_error);
		deleteLater();

		return;
	}

	if(error)
	{
		emit transferFailed( tr("A network error has occurred.") );
		deleteLater();

		return;
	}

	foreach(QVariant action, mResponse)
	{
		QVariantMap map = action.toMap();

		emit transferFinished(map, map.value("user_data").toString());
	}

	deleteLater();
}

void localTransfer::readyRead()
{
	if(!mHaveHeader)
	{
		while( mLocal->canReadLine() )
		{
			QString line = QString::fromUtf8( mLocal->readLine() );
			mHeader += line;

			if( line.trimmed().isEmpty() )
			{
				QHttpResponseHeader header(mHeader);

				mContentLength = header.contentLength();
				mHaveHeader = true;

				responseHeaderReceived(header);

				break;
			}
		}
	}

	if(mHaveHeader)
	{
		mContent += mLocal->readAll();
		if(mContent.size() >= mContentLength)
			requestFinished(false);
	}
}

void localTransfer::responseHeaderReceived(const QHttpResponseHeader& resp)
{
	if(resp.statusCode() != 200)
	{
		emit transferFailed( tr("Server returned code %1").arg(
			resp.statusCode() ) );
		deleteLater();

		return;
	}

	if( !resp.hasKey("X-JSON") )
	{
		emit transferFailed( tr("Response missing X-JSON header") );
		deleteLater();

		return;
	}

	QRegExp jsonResponse("\\((.*)\\)");
	QString result = resp.value("X-JSON").toAscii();

	if( jsonResponse.exactMatch(result) )
		result = jsonResponse.cap(1);

	// TODO: Add error checking here
	mResponse = json::parse(result).toList();

	QListIterator<QVariant> it(mResponse);
	while( it.hasNext() )
	{
		QVariantMap map = it.next().toMap();
		if( map.contains("error") )
		{
			emit transferFailed( map.value("error").toString() );
			deleteLater();

			return;
		}
	}
}
