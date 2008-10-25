/******************************************************************************\
*  server/src/DatabaseActions.cpp                                              *
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

#include "DatabaseActions.h"
#include "Utils.h"
#include "Log.h"

#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <QtCore/QStringList>

QVariantMap dbActionExport(int i, QTcpSocket *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(email);
	Q_UNUSED(action);
	Q_UNUSED(connection);

	// TODO: Restrict this to "admin" only

	QVariantMap export_tables;

	QStringList tables = db.tables();
	foreach(QString table, tables)
	{
		QStringList columns = cache_columns(table, db);

		QString sql = QString("SELECT %1 FROM %2").arg(
			columns.join(", ") ).arg(table);

		QSqlQuery query(sql, db);
		if( !query.exec() )
		{
			// Error
		}

		QVariantList rows;
		while( query.next() )
		{
			QSqlRecord record = query.record();

			QVariantMap row;
			QListIterator<QString> it(columns);
			while( it.hasNext() )
			{
				QString column = it.next();
				row[column] = record.value(column);
			}
			rows << row;
		}

		export_tables[table] = rows;
	}

	QVariantMap map;
	map["export"] = export_tables;

	return map;
}

QVariantMap dbActionImport(int i, QTcpSocket *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(email);
	Q_UNUSED(connection);

	if( !action.contains("export") )
	{
		LOG_ERROR("Missing param 'export'");
	}

	// TODO: Add error checking and transactions
	QSqlDatabase m_db = db;
	m_db.transaction();

	QVariantMap tables = action.value("export").toMap();
	QMapIterator<QString, QVariant> it(tables);

	while( it.hasNext() )
	{
		it.next();

		QVariantList rows = it.value().toList();
		QListIterator<QVariant> rows_it(rows);

		while( rows_it.hasNext() )
		{
			QVariant row = rows_it.next();

			QVariantMap map = row.toMap();
			QStringList columns = map.keys();
			QStringList binds;
			foreach(QString column, columns)
				binds << QString(":%1").arg(column);

			QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(
				it.key() ).arg( columns.join(", ") ).arg( binds.join(", ") );

			QSqlQuery query(db);
			if( !query.prepare(sql) )
			{
				LOG_ERROR(sql);
				LOG_ERROR( query.lastError().text() );

				m_db.rollback();

				return QVariantMap();
			}

			foreach(QString column, columns)
				query.bindValue(QString(":%1").arg(column), map.value(column));

			if( !query.exec() )
			{
				LOG_ERROR(sql);
				LOG_ERROR( query.lastError().text() );
				m_db.rollback();

				return QVariantMap();
			}
		}
	}

	m_db.commit();

	return QVariantMap();
}
