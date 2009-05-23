/******************************************************************************\
*  client/src/ajaxTransfer.cpp                                                 *
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

#include "ajaxTransfer.h"
#include "ajax.h"
#include "json.h"

#include <QtCore/QStringList>
#include <QtGui/QMessageBox>

ajaxTransfer::ajaxTransfer(QObject *parent_object) : baseTransfer(parent_object)
{
	// Nothing to see here
}

ajaxTransfer::~ajaxTransfer()
{
	if(mHttp)
	{
		delete mHttp;
		mHttp = 0;
	}
}

void ajaxTransfer::sslErrors(const QList<QSslError>& errors)
{
	QStringList list;
	foreach(QSslError err, errors)
		list << err.errorString();

	QMessageBox::critical(0, tr("AJAX Error"), tr("The AJAX backend has "
		"encountered the following SSL errors:\n%1").arg( list.join("\n") ));
}

ajaxTransfer* ajaxTransfer::start(const QUrl& url,
	const QMap<QString, QString>& post)
{
	ajaxTransfer *transfer = new ajaxTransfer;

	transfer->mHttp = new QHttp(url.host(),
		(url.scheme() == "https") ? QHttp::ConnectionModeHttps :
		QHttp::ConnectionModeHttp, url.port(80), 0);

	connect(transfer->mHttp, SIGNAL(sslErrors(const QList<QSslError>&)),
		transfer, SLOT(sslErrors(const QList<QSslError>&)));

	transfer->mBackendURL = url;

	QHttpRequestHeader header(post.isEmpty() ? "GET" : "POST", url.path());

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
	header.addValue("Accept-Encoding", "gzip");
	header.addValue("X-FrostyRPC-Version", "1.0");
	header.addValue("Host", url.host());

	connect( transfer->mHttp, SIGNAL(requestFinished(int, bool)),
		transfer, SLOT(requestFinished(int, bool)), Qt::QueuedConnection );

	connect( transfer->mHttp, SIGNAL(readyRead(const QHttpResponseHeader&)),
		transfer, SLOT(readyRead(const QHttpResponseHeader&)) );

	connect( transfer->mHttp,
		SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)),
		transfer, SLOT(responseHeaderReceived(const QHttpResponseHeader&)) );

	transfer->mRequestID = transfer->mHttp->request(header, data);

	return transfer;
}

void ajaxTransfer::requestFinished(int id, bool error)
{
	if(mRequestID != id)
		return;

	if(mUsingGzip)
		inflateEnd(&mStream);

	if(error)
	{
		emit transferFailed( tr("A network error has occurred.") );
		deleteLater();

		return;
	}

	QString result = QString::fromUtf8(mContent);

	mResponse = json::parse(result, false).toList();
	if( mResponse.isEmpty() )
	{
		emit transferFailed(result);
		deleteLater();

		return;
	}

	QListIterator<QVariant> it(mResponse);
	while( it.hasNext() )
	{
		QVariantMap map = it.next().toMap();
		if( map.contains("error") )
			emit transferFailed(map.value("error").toString());
		else
			emit transferFinished(map, map.value("user_data").toString());
	}

	emit transferInfo(result, mResponse);

	deleteLater();
}

void ajaxTransfer::readyRead(const QHttpResponseHeader& resp)
{
	Q_UNUSED(resp);

	if(mUsingGzip)
	{
		QByteArray buffer = mHttp->readAll();

		char out_buffer[4096];

		mStream.next_in = (Bytef*)buffer.data();
		mStream.avail_in = buffer.size();

		while(true)
		{
			mStream.next_out = (Bytef*)out_buffer;
			mStream.avail_out = sizeof(out_buffer);

			int ret = inflate(&mStream, Z_SYNC_FLUSH);
			int written = sizeof(out_buffer) - mStream.avail_out;

#if QT_VERSION >= 0x040500
			mContent.append(out_buffer, written);
#else
			mContent.append( QByteArray( out_buffer, written) );
#endif

			if(ret != Z_OK)
				break;

			// We are done reading what we have so far
			if(mStream.avail_in == 0)
				break;
		}
	}
	else
	{
		mContent += mHttp->readAll();
	}
}

void ajaxTransfer::responseHeaderReceived(const QHttpResponseHeader& resp)
{
	if(resp.statusCode() != 200)
	{
		emit transferFailed( tr("Server returned code %1").arg(
			resp.statusCode() ) );
		deleteLater();

		return;
	}

	if( resp.hasKey("Content-Encoding") && resp.value("Content-Encoding"
		).trimmed().compare("gzip", Qt::CaseInsensitive) == 0 )
	{
		mUsingGzip = true;

		memset(&mStream, 0, sizeof(mStream));
		inflateInit2(&mStream, MAX_WBITS + 16);
	}
}
