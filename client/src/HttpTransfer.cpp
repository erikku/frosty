/******************************************************************************\
*  updater/src/HttpTransfer.cpp                                                *
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

#include "HttpTransfer.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtGui/QMessageBox>

HttpTransfer::HttpTransfer(QObject *parent_object) : QObject(parent_object)
{
	memset(&mStream, 0, sizeof(mStream));
}

HttpTransfer::~HttpTransfer()
{
	if(mHttp)
	{
		delete mHttp;
		mHttp = 0;
	}
}

HttpTransfer* HttpTransfer::start(const QUrl& url, const QString& path,
	const QMap<QString, QString>& post, bool bzip2)
{
	HttpTransfer *transfer = new HttpTransfer;

	transfer->mHttp = new QHttp(url.host(),
		QHttp::ConnectionModeHttp, url.port(80), 0);

	transfer->mSourceURL = url;
	transfer->mDestPath = path;
	transfer->mDestHandle = 0;
	transfer->mUsingBzip2 = bzip2;

	QHttpRequestHeader header;
	if(bzip2)
	{
		BZ2_bzDecompressInit(&transfer->mStream, 0, 0);

		header = QHttpRequestHeader(post.isEmpty() ?
			"GET" : "POST", url.path() + ".bz2");
	}
	else
	{
		header = QHttpRequestHeader(post.isEmpty() ?
			"GET" : "POST", url.path());
	}

	sha1_starts(&transfer->mChecksumContext);

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

		header.setContentType("application/x-www-form-urlencoded; charset=UTF-8");
		header.setContentLength( data.length() );
	}

	header.addValue("User-Agent", "Qt4 JSON");
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

void HttpTransfer::requestFinished(int id, bool error)
{
	if(mRequestID != id)
		return;

	if(!mDestHandle)
	{
		QMessageBox::warning(0, tr("HTTP Warning"), tr("requestFinished() "
			"called without file handle."));

		return;
	}

	Q_UNUSED(error);

	if(mUsingBzip2)
		BZ2_bzDecompressEnd(&mStream);

	mDestHandle->close();
	delete mDestHandle;
	mDestHandle = 0;

	unsigned char hash[20];
	sha1_finish(&mChecksumContext, hash);

	QString checksum;
	for(int i = 0; i < 20; i++)
	{
		int byte = (int)hash[i];
		checksum += QString("%1").arg(byte, 2, 16, QLatin1Char('0'));
	}

	emit transferFinished(checksum);
}

void HttpTransfer::readyRead(const QHttpResponseHeader& resp)
{
	if(!mDestHandle)
	{
		QMessageBox::warning(0, tr("HTTP Warning"), tr("readyRead() called "
			"without file handle."));

		return;
	}

	Q_UNUSED(resp);

	QByteArray buffer = mHttp->readAll();

	if(mUsingBzip2)
	{
		mStream.next_in = buffer.data();
		mStream.avail_in = buffer.size();

		while(true)
		{
			mStream.next_out = mBufferOut;
			mStream.avail_out = sizeof(mBufferOut);

			int ret = BZ2_bzDecompress(&mStream);
			int written = sizeof(mBufferOut) - mStream.avail_out;

			sha1_update(&mChecksumContext, (uchar*)mBufferOut, written);
			mDestHandle->write(mBufferOut, written);
			mWritten += written;

			if(ret != BZ_OK)
				break;

			// We are done reading what we have so far
			if(mStream.avail_in == 0)
				break;
		}
	}
	else
	{
		sha1_update(&mChecksumContext, (uchar*)buffer.data(), buffer.size());
		mDestHandle->write(buffer);
		mWritten += buffer.size();
	}

	emit progressChanged( qRound((double)mWritten /
		(double)mContentLength * 100.0f) );
}

void HttpTransfer::responseHeaderReceived(const QHttpResponseHeader& resp)
{
	if(resp.statusCode() != 200)
	{
		QMessageBox::critical(0, tr("HTTP Error"),
			tr("Server returned code %1").arg(resp.statusCode()));

		emit transferFailed();

		return;
	}

	mDestHandle = new QFile(mDestPath);
	if(!mDestHandle)
	{
		emit transferFailed();

		return;
	}

	if( !QFileInfo(mDestPath).path().isEmpty() )
	{
		if( !QDir(QDir::currentPath() + QString("/") +
			QFileInfo(mDestPath).path()).exists() &&
			!QDir::current().mkpath( QFileInfo(mDestPath).path() ) )
		{
				QMessageBox::critical(0, tr("I/O Error"), tr("Failed to create "
					"directory '%1'.").arg( QFileInfo(mDestPath).path() ) );

				delete mDestHandle;
				mDestHandle = 0;

				emit transferFailed();

				return;
		}
	}

	if( !mDestHandle->open(QIODevice::WriteOnly) )
	{
		QMessageBox::critical(0, tr("I/O Error"), tr("Failed to open file '%1' "
			"for writing.").arg( QFileInfo(mDestPath).baseName() ) );

		delete mDestHandle;
		mDestHandle = 0;

		emit transferFailed();

		return;
	}

	mWritten = 0;
	mContentLength = resp.value("Content-Length").toInt();

	emit progressChanged(0);
}
