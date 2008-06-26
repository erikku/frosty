/******************************************************************************\
*  client/src/ajaxTransfer.h                                                   *
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

#ifndef __ajaxTransfer_h__
#define __ajaxTransfer_h__

#include <QtCore/QUrl>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QByteArray>
#include <QtNetwork/QHttp>

#include "sha1.h"

typedef QMap<QString, QString> QStringMap;

class ajaxTransfer : public QObject
{
	Q_OBJECT

public:
	~ajaxTransfer();

	static ajaxTransfer* start(const QUrl& url,
		const QMap<QString, QString>& post = QStringMap());

protected slots:
	void requestFinished(int id, bool error);
	void readyRead(const QHttpResponseHeader& resp);
	void responseHeaderReceived(const QHttpResponseHeader& resp);

signals:
	void transferFailed();
	void transferFinished(const QVariant& response);
	void transferInfo(const QString& content, const QVariant& response);

protected:
	ajaxTransfer(QObject *parent = 0);

	QHttp *mHttp;
	int mRequestID;
	QUrl mBackendURL;
	QVariant mResponse;
	QByteArray mContent;
};

#endif // __ajaxTransfer_h__
