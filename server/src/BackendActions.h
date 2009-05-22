/******************************************************************************\
*  server/src/BackendActions.h                                                 *
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

#ifndef __BackendActions_h__
#define __BackendActions_h__

#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtSql/QSqlDatabase>

class QIODevice;

QVariantMap backendActionSalt(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email);
QVariantMap backendActionDelete(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email);
QVariantMap backendActionInsert(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email);
QVariantMap backendActionUpdate(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email);
QVariantMap backendActionSelect(int i, QIODevice *connection,
	const QSqlDatabase& db, const QSqlDatabase& user_db,
	const QVariantMap& action, const QString& email);

#endif // __BackendActions_h__
