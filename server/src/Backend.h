/******************************************************************************\
*  server/src/Backend.h                                                        *
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

#ifndef __Backend_h__
#define __Backend_h__

#include <QtSql/QSqlDatabase>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QList>
#include <QtCore/QMap>

class QTcpSocket;

typedef QVariantMap (*BackendActionHandler)(int, QTcpSocket*,
	const QSqlDatabase&, const QVariantMap&, const QString&);

class Backend : public QObject
{
public:
	Backend();

	QVariantList parseRequest(QTcpSocket *connection,
		const QSqlDatabase& db, const QMap<QString, QString>& post) const;

protected:
	QMap<QString, BackendActionHandler> mActionHandlers;
};

#endif // __Backend_h__
