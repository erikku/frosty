/******************************************************************************\
*  updater/src/HttpTransfer.h                                                  *
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

#ifndef __HttpTransfer_h__
#define __HttpTransfer_h__

#include <QtCore/QUrl>
#include <QtCore/QMap>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtNetwork/QHttp>
#include <bzlib.h>

#include "sha1.h"

typedef QMap<QString, QString> QStringMap;

class HttpTransfer : public QObject
{
	Q_OBJECT

public:
	~HttpTransfer();

	static HttpTransfer* start(const QUrl& url, const QString& path,
		const QMap<QString, QString>& post = QStringMap(), bool bzip2 = false);

protected slots:
	void requestFinished(int id, bool error);
	void readyRead(const QHttpResponseHeader& resp);
	void responseHeaderReceived(const QHttpResponseHeader& resp);

signals:
	void transferFailed();
	void progressChanged(int progress);
	void transferFinished(const QString& checksum);

protected:
	HttpTransfer(QObject *parent = 0);

	QHttp *mHttp;
	int mWritten;
	int mRequestID;
	QUrl mSourceURL;
	QString mDestPath;
	int mContentLength;
	QFile *mDestHandle;
	sha1_context mChecksumContext;

	bool mUsingBzip2;
	bz_stream mStream;
	char mBufferOut[4096];
};

#endif // __HttpTransfer_h__
