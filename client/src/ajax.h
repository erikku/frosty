/******************************************************************************\
*  client/src/ajax.h                                                           *
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

#ifndef __ajax_h__
#define __ajax_h__

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QUrl>

#include <QtNetwork/QHttpResponseHeader>

class QTimer;
class LogWidget;

class ajax : public QObject
{
	Q_OBJECT

public:
	static ajax* getSingletonPtr();

	void request(const QUrl& url, const QVariant& request);
	void subscribe(QObject *obj);

public slots:
	void showLog();
	void dispatchQueue(const QUrl& url = QUrl());

signals:
	void response(const QVariant& response);

protected:
	ajax(QObject *parent = 0);
	~ajax();

	void dispatchRequest(const QUrl& url, const QVariant& request);

	LogWidget *mLog;

	QTimer *mRequestQueueTimer;
	QMap<QUrl, QVariantList> mRequestQueue;
};

#endif // __ajax_h__
