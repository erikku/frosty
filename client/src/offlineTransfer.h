/******************************************************************************\
*  client/src/offlineTransfer.h                                                *
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

#ifndef __offlineTransfer_h__
#define __offlineTransfer_h__

#include "baseTransfer.h"

#include <QtSql/QSqlDatabase>

class Backend;

#ifndef QSTRINGMAP_DEF
#define QSTRINGMAP_DEF
typedef QMap<QString, QString> QStringMap;
#endif // QSTRINGMAP_DEF

class offlineTransfer : public baseTransfer
{
	Q_OBJECT

public:
	~offlineTransfer();

	static offlineTransfer* inst();

	static offlineTransfer* start(const QVariant& req);

protected:
	offlineTransfer(QObject *parent = 0);

	offlineTransfer* startReal(const QVariant& req);

	QString mEmail;
	QSqlDatabase db, user_db;

	Backend *mBackend;
};

#endif // __offlineTransfer_h__
