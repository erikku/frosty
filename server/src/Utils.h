/******************************************************************************\
*  server/src/Utils.h                                                          *
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

#ifndef __Utils_h__
#define __Utils_h__

#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtSql/QSqlDatabase>

QString sha1FromString(const QString& str);

QVariantMap herror(const QString& type, const QString& err);
QVariantMap herror_sql(const QSqlDatabase& db, const QString& type,
	const QString& err);
QVariantMap param_error(const QString& type, const QString& param,
	const QString& action);

QStringList tables_blacklist();
QMap<QString, QString> where_connect_types();
QMap<QString, QString> where_types();

QString full_column_name(const QString& table, const QString& column);
QStringList cache_columns(const QString& table, const QSqlDatabase& db);
bool check_column(const QString& col, const QString& table,
	const QSqlDatabase& db);

#endif // __Utils_h__
