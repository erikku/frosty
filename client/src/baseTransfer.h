/******************************************************************************\
*  client/src/baseTransfer.h                                                   *
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

#ifndef __baseTransfer_h__
#define __baseTransfer_h__

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariant>

class baseTransfer : public QObject
{
	Q_OBJECT

public:
	~baseTransfer() { };

signals:
	void transferFailed(const QString& err);
	void transferFinished(const QVariantMap& response,
		const QString& user_data);
	void transferInfo(const QString& content, const QVariantList& response);

protected:
	baseTransfer(QObject *parent = 0) : QObject(parent) { };
};

#endif // __baseTransfer_h__
