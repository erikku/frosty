/******************************************************************************\
*  client/src/localTransfer.h                                                  *
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

#ifndef __localTransfer_h__
#define __localTransfer_h__

#include "baseTransfer.h"

#include <QtCore/QPointer>
#include <QtCore/QByteArray>

class QLocalSocket;
class QHttpResponseHeader;

#include "sha1.h"

#ifndef QSTRINGMAP_DEF
#define QSTRINGMAP_DEF
typedef QMap<QString, QString> QStringMap;
#endif // QSTRINGMAP_DEF

class localTransfer : public baseTransfer
{
	Q_OBJECT

public:
	~localTransfer();

	static localTransfer* start(
		const QMap<QString, QString>& post = QStringMap());

protected slots:
	void readyRead();
	void dispatchError();

protected:
	localTransfer(QObject *parent = 0);

	void error(const QString& error);
	void requestFinished(bool error);
	void responseHeaderReceived(const QHttpResponseHeader& resp);

	bool mHaveHeader;
	int mContentLength;

	QString mError;
	QString mHeader;
	QByteArray mContent;

	QVariantList mResponse;
	QPointer<QLocalSocket> mLocal;
};

#endif // __localTransfer_h__
