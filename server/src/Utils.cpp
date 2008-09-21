/******************************************************************************\
*  server/src/Utils.cpp                                                        *
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

#include "Utils.h"
#include "sha1.h"

#include <QtCore/QObject>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

#define tr(msg) QObject::tr(msg)

QString sha1FromString(const QString& str)
{
	QString hash;
	QByteArray data = str.toUtf8();

	unsigned char checksum[20];

	sha1((uchar*)data.data(), data.size(), checksum);

	for(int i = 0; i < 20; i++)
		hash += QString("%1").arg(((int)checksum[i]), 2, 16, QLatin1Char('0'));

	return hash;
};

QVariantMap herror(const QString& type, const QString& err)
{
	QVariantMap map;
	map["error"] = tr("Error parsing %1: %2").arg(type).arg(err);

	return map;
};

QVariantMap herror_sql(const QSqlDatabase& db, const QString& type,
	const QString& err)
{
	QSqlDatabase m_db = db;
	m_db.rollback();

	QVariantMap map;
	map["error"] = tr("Error parsing %1: %2").arg(type).arg(err);

	return map;
};

QVariantMap param_error(const QString& type, const QString& param,
	const QString& action)
{
	QVariantMap map;
	map["error"] = tr("Error parsing %1: paramater '%2' for %3 is missing").arg(
		type).arg(param).arg(action);

	return map;
};

QStringList tables_blacklist()
{
	QStringList blacklist;
	blacklist << "users";

	return blacklist;
};

QString full_column_name(const QString& table, const QString& column)
{
	if( column.contains('.') )
		return column;

	return QString("%1.%2").arg(table).arg(column);
};

QStringList cache_columns(const QString& table, const QSqlDatabase& db)
{
	static QMap<QString, QMap<QString, QStringList> > master_cache;

	QMap<QString, QStringList> cache = master_cache.value(
		db.connectionName() );

	if( cache.contains(table) )
		return cache.value(table);

	QString sql;
	if(db.driverName() == "QMYSQL")
		sql = QString("DESCRIBE %1").arg(table);
	else // SQLite
		sql = QString("PRAGMA table_info(%1)").arg(table);

	QSqlQuery query(sql, db);
	if( !query.exec() )
		return QStringList();

	QStringList list;
	if(db.driverName() == "QMYSQL")
	{
		while( query.next() )
			list << query.record().value("Field").toString();
	}
	else // SQLite
	{
		while( query.next() )
			list << query.value(1).toString();
	}


	// Save the column info to the cache
	cache[table] = list;

	// Save the cache to the master cache (map of database connections)
	master_cache[db.connectionName()] = cache;

	return list;
};

QMap<QString, QString> where_connect_types()
{
	QMap<QString, QString> types;
	types["or"]  =  " OR ";
	types["and"] = " AND ";

	return types;
};

QMap<QString, QString> where_types()
{
	QMap<QString, QString> types;
	types["greater_then"] = ">";
	types["less_then"]    = "<";
	types["equals"]       = "=";

	return types;
};

bool check_column(const QString& col, const QString& table,
	const QSqlDatabase& db)
{
	QStringList column_cache;
	QString real_col = col;

	if( col.contains('.') )
	{
		QString real_table = col.split('.').first();
		QString real_col = col.split('.').at(1);

		column_cache = cache_columns(real_table, db);
	}
	else
	{
		column_cache = cache_columns(table, db);
	}

	return column_cache.contains(real_col);
};
