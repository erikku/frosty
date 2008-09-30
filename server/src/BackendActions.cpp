/******************************************************************************\
*  server/src/BackendActions.cpp                                               *
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

#include "BackendActions.h"
#include "Config.h"
#include "Utils.h"
#include "Auth.h"
#include "Log.h"

#include <QtCore/QObject>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#define tr(msg) QObject::tr(msg)

QVariantMap backendActionSalt(int i, QTcpSocket *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(i);
	Q_UNUSED(db);
	Q_UNUSED(email);
	Q_UNUSED(action);
	Q_UNUSED(connection);

	QVariantMap map;
	map["salt"] = conf->saltPass();

	return map;
};

QVariantMap backendActionDelete(int i, QTcpSocket *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(email);
	Q_UNUSED(connection);

	// Check for the 'table' paramater
	if( !action.contains("table") )
		return param_error(tr("delete action"), tr("table"),
			tr("action %1").arg(i));

	// To make life easier
	QString table = action.value("table").toString();

	if( tables_blacklist().contains(table) || !db.tables().contains(table) )
		return herror(tr("delete action"), tr("table '%1' for action %2 does "
			"not exist").arg(table).arg(i));

	QString where;
	QVariantList where_values;
	QString where_connect_type = " AND ";

	if( action.contains("where_type") )
	{
		where_connect_type = action.value("where_type").toString().toLower();

		if( !where_connect_types().contains(where_connect_type) )
			return herror("delete action", tr("invalid where type "
				"'%1' for action %2").arg(where_connect_type).arg(i));

		where_connect_type = where_connect_types().value(where_connect_type);
	}

	if( action.contains("where") )
	{
		// TODO: More type-checking like this
		if( !action.value("where").canConvert(QVariant::List) )
			return herror("delete action", tr("paramater 'where' for action %1 "
				"exists but is not an array").arg(i));

		QVariantList where_list = action.value("where").toList();

		if(where_list.count() < 1)
			return herror("delete action", tr("paramater 'where' for action %1 "
				"exists but contains no where expressions").arg(i));

		QString where_type;
		QStringList where_expr;

		for(int j = 0; j < where_list.count(); j++)
		{
			QVariantMap expr = where_list.at(j).toMap();

			if( !expr.contains("type") )
				return param_error("delete action", "type", tr("where "
					"expression %1 in action %2").arg(j).arg(i));

			// TODO: Make make it so this changes instead of just replacing
			// the old type with the new one
			where_type = expr.value("type").toString().toLower();

			if( !where_types().contains(where_type)  )
				return herror("delete action", tr("where expression type %1 "
					"does not exists for where expression %2 in action "
					"%3").arg(where_type).arg(j).arg(i));

			where_type = where_types().value(where_type);

			if( !expr.contains("column") )
				return param_error("delete action", "column", tr("where "
					"expression %1 in action %2").arg(j).arg(i));

			QString column = expr.value("column").toString();

			// Check all tables, including relation tables
			if( !check_column(column, table, db) )
				return herror("delete action", tr("invalid column "
					"'%1' for where expression %2 in action %3").arg(
					expr.value("column").toString()).arg(j).arg(i));

			if( !expr.contains("value") )
				return param_error("delete action", "value", tr("where "
					"expression %1 in action %2").arg(j).arg(i));

			where_expr << QString("%1 %2 ?").arg(column).arg(where_type);
			where_values << expr.value("value");
		}

		where = QString(" WHERE %1").arg( where_expr.join(where_connect_type) );
	}

	QString order_by;

	if( action.contains("order_by") )
	{
		QVariantList order_by_list = action.value("order_by").toList();

		if(order_by_list.count() < 1)
			return herror("delete action", tr("paramater 'order_by' for action "
				"%1 exists but contains no order expressions").arg(i));

		QStringList order_by_expr;

		for(int j = 0; j < order_by_list.count(); j++)
		{
			QVariantMap order = order_by_list.at(j).toMap();

			if( !order.contains("column") )
				return param_error("delete action", "column", tr("order "
					"expression %1 in action %2").arg(j).arg(i));

			// MySQL error for action 1: Column 'id' in
			// where clause is ambiguous
			// i.e. make the column name full (table.column)
			QString column = order.value("column").toString();

			// Check all tables, including relation tables
			if( !check_column(column, table, db) )
				return herror("delete action", tr("invalid column "
					"'%1' for order expression %2 in action %3").arg(
					order.value("column").toString()).arg(j).arg(i));

			QString direction = "ASC";

			if( order.contains("direction") )
			{
				direction = order.value("direction").toString().toUpper();

				if(direction != "ASC" && direction != "DESC")
					return herror("delete action", tr("invalid direction "
						"'%1' for order expression %2 in action %3").arg(
						direction).arg(j).arg(i));
			}

			order_by_expr <<QString("%1 %2").arg(column).arg(direction);
		}

		order_by = QString(" ORDER BY %1").arg( order_by_expr.join(", ") );
	}

	QString limit;

	if( action.contains("limit") )
	{
		int limit_value = action.value("limit").toInt();

		if(limit_value < 1)
			return herror("delete action", tr("limit for in action %1 must be a "
				"positive integer").arg(i));

		if( action.contains("offset") )
		{
			QVariant offset = action.value("offset");
			int offset_value = limit.toInt();

			if(!offset.canConvert(QVariant::Int) || offset_value < 0)
				return herror("delete action", tr("offset must be a positive "
					"integer for action %1").arg(i));

			limit = QString(" LIMIT %1, %2").arg(offset_value).arg(limit_value);
		}
		else
		{
			limit = QString(" LIMIT %1").arg(limit_value);
		}
	}
	else if( action.contains("offset") )
	{
		return herror("delete action", "to include the paramater 'offset', "
			"you must also include the paramater 'limit'");
	}

	QSqlDatabase m_db = db;
	m_db.transaction();

	QString sql = QString("DELETE FROM %1%2%3%4").arg(table).arg(where).arg(
		order_by).arg(limit);

	QSqlQuery query(db);
	if( !query.prepare(sql) )
		return herror_sql(db, "delete action", tr("SQL error for action "
			"%1: %2").arg(i).arg( query.lastError().text() ));

	for(int j = 0; j < where_values.count(); j++)
		query.bindValue(j, where_values.at(j), QSql::In);

	if( !query.exec() )
		return herror_sql(db, "delete action", tr("SQL error for action "
			"%1: %2").arg(i).arg( query.lastError().text() ));

	m_db.commit();

	return QVariantMap();
};

QVariantMap backendActionInsert(int i, QTcpSocket *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(email);
	Q_UNUSED(connection);

	// Check for the 'table' paramater
	if( !action.contains("table") )
		return param_error(tr("insert action"), tr("table"),
			tr("action %1").arg(i));

	// To make life easier
	QString table = action.value("table").toString();

	if( tables_blacklist().contains(table) || !db.tables().contains(table) )
		return herror(tr("insert action"), tr("table '%1' for action %2 does "
			"not exist").arg(table).arg(i));

	// Check for the 'rows' paramater
	if( !action.contains("rows") )
		return param_error("insert action", "rows", tr("action %1").arg(i));

	// Check the 'rows' paramater is an array
	if( !action.value("rows").canConvert(QVariant::List) )
		return herror("insert action", tr("parmater 'rows' for action %1 is "
			"not an array").arg(i));

	QVariantList rows = action.value("rows").toList();

	// Check the 'rows' paramater is not empty
	if(rows.count() < 1)
		return herror("insert action", tr("no rows to insert for "
			"action %1").arg(i));

	QStringList relation_aliases;

	if( action.contains("relations") )
	{
		if( !action.value("relations").canConvert(QVariant::List) )
			return herror("insert action", tr("parmater 'relations' for action "
				"%1 is not an object").arg(i));

		QVariantMap relations = action.value("relations").toMap();

		if(relations.count() < 1)
			return herror("select action", tr("paramater 'relations' for "
				"action %1 exists but contains no relations").arg(i));

		foreach(QString foreign_table, relations.keys())
		{
			QVariantMap relation = relations.value(foreign_table).toMap();

			if( tables_blacklist().contains(foreign_table) ||
				!db.tables().contains(foreign_table) )
					return herror("select action", tr("table '%1' for "
						"relation '%2' in action %3 does not exist").arg(
						foreign_table).arg(foreign_table).arg(i));

			if( !relation.contains("id") )
				return param_error("select action", "id", tr("relation "
					"'%1' in action %2").arg(foreign_table).arg(i));

			QString relation_id = relation.value("id").toString();

			if( !check_column(relation_id, foreign_table, db) )
				return herror("select action", tr("column '%1' on table '%2' "
				"for relation '%3' in action %4 does not exist").arg(
				relation_id).arg(table).arg(foreign_table).arg(i));

			if( !relation.contains("foreign_id") )
				return param_error("select action", "foreign_id", tr("relation "
					"'%1' in action %2").arg(foreign_table).arg(i));

			QString foreign_id = relation.value("foreign_id").toString();

			if( !check_column(foreign_id, foreign_table, db) )
				return herror("select action", tr("column '%1' on table '%2' "
				"for relation '%3' in action %3 does not exist").arg(
				foreign_id).arg(foreign_table).arg(foreign_table).arg(i));

			if( !relation.contains("columns") )
				return param_error("select action", "columns", tr("relation "
					"'%1' in action %2").arg(foreign_table).arg(i));

			QVariantMap columns_and_aliases = relation.value("columns").toMap();

			if(columns_and_aliases.count() <= 0)
				return herror("select action", tr("paramater 'columns' for "
					"relation '%1' in action %2 exists but contains no "
					"columns").arg(foreign_table).arg(i));

			QStringList columns = cache_columns(table, db);

			foreach(QString relation_column, columns_and_aliases.keys())
			{
				QString alias = columns_and_aliases.value(
					relation_column).toString();

				if( !check_column(relation_column, foreign_table, db) )
					return herror("select action", tr("invalid column '%1' for "
					"relation '%2' in action %3").arg(
					relation_column).arg(foreign_table).arg(i));

				if( alias.trimmed().isEmpty() )
					alias = QString("%1_%2").arg(foreign_table).arg(
						relation_column);

				if(columns.contains(alias) || relation_aliases.contains(alias))
					return herror("select action", tr("alias '%1' of column"
 						"'%2' for relation '%3' in action %4 conflicts with an "
 						"existing column or alias").arg(alias).arg(
 						relation_column).arg(foreign_table).arg(i));

				relation_aliases << alias;
			}
		}
	}

	QStringList columns;
	foreach(QVariant row, rows)
	{
		foreach(QString key, row.toMap().keys())
		{
			if( !columns.contains(key) )
				columns << key;
		}
	}

	// Strip all columns that are for aliases
	{
		QStringList::iterator it = columns.begin();
		while( it != columns.end() )
		{
			if( relation_aliases.contains(*it) )
				it = columns.erase(it);
			else
				it++;
		}
	}

	// Check all columns
	foreach(QString column, columns)
	{
		if( !check_column(column, table, db) )
			return herror("insert action", tr("column '%1' for action %2 does "
				"not exist").arg(column).arg(i));
	}

	int row_count = rows.count();

	// Check all the columns are there
	for(int j = 0; j < row_count; j++)
	{
		QStringList row_columns = rows.at(j).toMap().keys();

		// Diff the arrays
		QStringList missing = relation_aliases + columns;

		QStringList::iterator it = missing.begin();
		while( it != missing.end() )
		{
			if( row_columns.contains(*it) )
				it = missing.erase(it);
			else
				it++;
		}

		if(missing.count() > 0)
			return herror("insert action", tr("column '%1' for row %2 in "
				"action %3 does not exist").arg(
				missing.first() ).arg(j).arg(i));
	}

	QSqlDatabase m_db = db;
	m_db.transaction();

	if( action.contains("relations") )
	{
		QVariantMap relations = action.value("relations").toMap();

		foreach(QString foreign_table, relations.keys())
		{
			QVariantMap relation = relations.value(foreign_table).toMap();

			QVariantMap columns_and_aliases = relation.value("columns").toMap();
			QStringList where_columns;

			// Prepare the bind names
			QMapIterator<QString, QVariant> it = columns_and_aliases;
			while( it.hasNext() )
			{
				it.next();

				QString alias = it.value().toString();
				if( alias.trimmed().isEmpty() )
					alias = QString("%1_%2").arg(foreign_table).arg(it.key());

				where_columns << QString("%1 = :%2").arg(it.key()).arg(alias);
			}

			QString foreign_id = relation.value("foreign_id").toString();
			QString sql = QString("SELECT %1 FROM %2 WHERE %3 LIMIT 1").arg(
				foreign_id).arg(foreign_table).arg(where_columns.join(" AND "));

			QSqlQuery query(db);
			query.prepare(sql);

			QString relation_id = relation.value("id").toString();

			for(int j = 0; j < row_count; j++)
			{
				it.toFront();
				while( it.hasNext() )
				{
					it.next();

					QString alias = it.value().toString();
					if( alias.trimmed().isEmpty() )
						alias = QString(":%1_%2").arg(foreign_table).arg(
							it.key());

					QVariant value = rows.at(j).toMap().value(alias);
					query.bindValue(QString(":%1").arg(alias), value);
				}

				if( !query.exec() )
					return herror_sql(db, "insert action", tr("SQL error for "
						"relation '%1' in action %2: %3").arg(
						foreign_table).arg(i).arg( query.lastError().text() ));

				QVariant id;
				if( !query.next() )
				{
					query.record().value(foreign_id);
				}
				else
				{
					// If we are allowed to insert the relations, do so
					if( relation.contains("insert") && relation.value(
						"insert").toString().toLower() == "true")
					{
						QString where_columns_set = where_columns.join(", ");

						QString sql = QString("INSERT INTO %1 SET %2").arg(
							foreign_table).arg(where_columns_set);

						QSqlQuery query2(db);
						query2.prepare(sql);

						it.toFront();
						while( it.hasNext() )
						{
							it.next();

							QString alias = it.value().toString();
							if( alias.trimmed().isEmpty() )
								alias = QString(":%1_%2").arg(
									foreign_table).arg(it.key());

							QVariant value = rows.at(j).toMap().value(alias);
							query.bindValue(QString(":%1").arg(alias), value);
						}

						if( !query.exec() )
							return herror_sql(db, "insert action", tr("SQL "
								"error for relation '%1' in action %2").arg(
								foreign_table).arg(i));

						id = query.lastInsertId();
					}
					else
					{
						return herror("insert action", tr("unable to get id for"
							" relation '%1' in action %2").arg(
							foreign_table).arg(i));
					}
				}

				// Save the id to the row
				QVariantMap row = rows.at(j).toMap();
				row[relation_id] = id;

				// Remove all the relation rows
				it.toFront();
				while( it.hasNext() )
				{
					it.next();

					QString alias = it.value().toString();
					if( alias.trimmed().isEmpty() )
						alias = QString(":%1_%2").arg(
							foreign_table).arg(it.key());

					row.remove(alias);
				}

				// Save the row
				rows[j] = row;
			}
		}
	}

	QStringList value_holders;
	columns = rows.first().toMap().keys();

	foreach(QString column, columns)
		value_holders << QString(":%1").arg(column);

	QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(
		table).arg( columns.join(", ") ).arg( value_holders.join(", ") );

	QSqlQuery query(db);
	query.prepare(sql);

	QVariantList ids;
	for(int j = 0; j < row_count; j++)
	{
		QVariantMap row = rows.at(j).toMap();
		QMapIterator<QString, QVariant> it = row;

		while( it.hasNext() )
		{
			it.next();
			query.bindValue(QString(":%1").arg(it.key()), it.value());
		}

		if( !query.exec() )
			return herror_sql(db, "insert action", tr("SQL error for row %1 in "
				"action %2: %3").arg(j).arg(j).arg( query.lastError().text() ));

		ids << query.lastInsertId();
	}

	m_db.commit();

	QVariantMap map;
	map["ids"] = ids;

	return map;
};

QVariantMap backendActionUpdate(int i, QTcpSocket *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(email);
	Q_UNUSED(connection);

	// Check for the 'table' paramater
	if( !action.contains("table") )
		return param_error(tr("update action"), tr("table"),
			tr("action %1").arg(i));

	// To make life easier
	QString table = action.value("table").toString();

	if( tables_blacklist().contains(table) || !db.tables().contains(table) )
		return herror(tr("update action"), tr("table '%1' for action %2 does "
			"not exist").arg(table).arg(i));

	// Check for the 'rows' paramater
	if( !action.contains("rows") )
		return param_error("update action", "rows", tr("action %1").arg(i));

	// Check the 'rows' paramater is an array
	if( !action.value("rows").canConvert(QVariant::List) )
		return herror("update action", tr("parmater 'rows' for action %1 is "
			"not an array").arg(i));

	QVariantList rows = action.value("rows").toList();

	// Check the 'rows' paramater is not empty
	if(rows.count() < 1)
		return herror("update action", tr("no rows to update for "
			"action %1").arg(i));

	QStringList relation_aliases;

	if( action.contains("relations") )
	{
		if( !action.value("relations").canConvert(QVariant::List) )
			return herror("update action", tr("parmater 'relations' for action "
				"%1 is not an object").arg(i));

		QVariantMap relations = action.value("relations").toMap();

		if(relations.count() < 1)
			return herror("update action", tr("paramater 'relations' for "
				"action %1 exists but contains no relations").arg(i));

		foreach(QString foreign_table, relations.keys())
		{
			QVariantMap relation = relations.value(foreign_table).toMap();

			if( tables_blacklist().contains(foreign_table) ||
				!db.tables().contains(foreign_table) )
					return herror("insert action", tr("table '%1' for "
						"relation '%2' in action %3 does not exist").arg(
						foreign_table).arg(foreign_table).arg(i));

			if( !relation.contains("id") )
				return param_error("insert action", "id", tr("relation "
					"'%1' in action %2").arg(foreign_table).arg(i));

			QString relation_id = relation.value("id").toString();

			if( !check_column(relation_id, foreign_table, db) )
				return herror("insert action", tr("column '%1' on table '%2' "
				"for relation '%3' in action %4 does not exist").arg(
				relation_id).arg(table).arg(foreign_table).arg(i));

			if( !relation.contains("foreign_id") )
				return param_error("insert action", "foreign_id", tr("relation "
					"'%1' in action %2").arg(foreign_table).arg(i));

			QString foreign_id = relation.value("foreign_id").toString();

			if( !check_column(foreign_id, foreign_table, db) )
				return herror("insert action", tr("column '%1' on table '%2' "
				"for relation '%3' in action %3 does not exist").arg(
				foreign_id).arg(foreign_table).arg(foreign_table).arg(i));

			if( !relation.contains("columns") )
				return param_error("insert action", "columns", tr("relation "
					"'%1' in action %2").arg(foreign_table).arg(i));

			QVariantMap columns_and_aliases = relation.value("columns").toMap();

			if(columns_and_aliases.count() <= 0)
				return herror("insert action", tr("paramater 'columns' for "
					"relation '%1' in action %2 exists but contains no "
					"columns").arg(foreign_table).arg(i));

			QStringList columns = cache_columns(table, db);

			foreach(QString relation_column, columns_and_aliases.keys())
			{
				QString alias = columns_and_aliases.value(
					relation_column).toString();

				if( !check_column(relation_column, foreign_table, db) )
					return herror("insert action", tr("invalid column '%1' for "
					"relation '%2' in action %3").arg(
					relation_column).arg(foreign_table).arg(i));

				if( alias.trimmed().isEmpty() )
					alias = QString("%1_%2").arg(foreign_table).arg(
						relation_column);

				if(columns.contains(alias) || relation_aliases.contains(alias))
					return herror("insert action", tr("alias '%1' of column"
 						"'%2' for relation '%3' in action %4 conflicts with an "
 						"existing column or alias").arg(alias).arg(
 						relation_column).arg(foreign_table).arg(i));

				relation_aliases << alias;
			}
		}
	}

	QStringList columns;
	foreach(QVariant row, rows)
	{
		foreach(QString key, row.toMap().keys())
		{
			if( !columns.contains(key) )
				columns << key;
		}
	}

	// Strip all columns that are for aliases
	{
		QStringList::iterator it = columns.begin();
		while( it != columns.end() )
		{
			if( relation_aliases.contains(*it) )
				it = columns.erase(it);
			else
				it++;
		}
	}

	// Check all columns
	foreach(QString column, columns)
	{
		if( !check_column(column, table, db) )
			return herror("update action", tr("column '%1' for action %2 does "
				"not exist").arg(column).arg(i));
	}

	int row_count = rows.count();

	// Check all the columns are there
	for(int j = 0; j < row_count; j++)
	{
		QStringList row_columns = rows.at(j).toMap().keys();

		// Diff the arrays
		QStringList missing = relation_aliases + columns;

		QStringList::iterator it = missing.begin();
		while( it != missing.end() )
		{
			if( row_columns.contains(*it) )
				it = missing.erase(it);
			else
				it++;
		}

		if(missing.count() > 0)
			return herror("update action", tr("column '%1' for row %2 in "
				"action %3 does not exist").arg(
				missing.first() ).arg(j).arg(i));
	}

	QString where;
	QVariantMap where_values;
	QString where_connect_type = " AND ";

	if( action.contains("where_type") )
	{
		where_connect_type = action.value("where_type").toString().toLower();

		if( !where_connect_types().contains(where_connect_type) )
			return herror("update action", tr("invalid where type "
				"'%1' for action %2").arg(where_connect_type).arg(i));

		where_connect_type = where_connect_types().value(where_connect_type);
	}

	if( action.contains("where") )
	{
		// TODO: More type-checking like this
		if( !action.value("where").canConvert(QVariant::List) )
			return herror("update action", tr("paramater 'where' for action %1 "
				"exists but is not an array").arg(i));

		QVariantList where_list = action.value("where").toList();

		if(where_list.count() < 1)
			return herror("update action", tr("paramater 'where' for action %1 "
				"exists but contains no where expressions").arg(i));

		QString where_type;
		QStringList where_expr;

		for(int j = 0; j < where_list.count(); j++)
		{
			QVariantMap expr = where_list.at(j).toMap();

			if( !expr.contains("type") )
				return param_error("update action", "type", tr("where "
					"expression %1 in action %2").arg(j).arg(i));

			// TODO: Make make it so this changes instead of just replacing
			// the old type with the new one
			where_type = expr.value("type").toString().toLower();

			if( !where_types().contains(where_type)  )
				return herror("update action", tr("where expression type %1 "
					"does not exists for where expression %2 in action "
					"%3").arg(where_type).arg(j).arg(i));

			where_type = where_types().value(where_type);

			if( !expr.contains("column") )
				return param_error("update action", "column", tr("where "
					"expression %1 in action %2").arg(j).arg(i));

			// MySQL error for action 1: Column 'id' in
			// where clause is ambiguous
			// i.e. make the column name full (table.column)
			QString column = expr.value("column").toString();
			// TODO: Check that this won't be an issue
			/*
			if( !relation_join_sql.isEmpty() ||
				relation_aliases.contains(column) )
					column = full_column_name(table, column);
			*/

			// Check all tables, including relation tables
			if( !check_column(column, table, db) &&
				!relation_aliases.contains( expr.value("column").toString() ) )
					return herror("update action", tr("invalid column "
						"'%1' for where expression %2 in action %3").arg(
						expr.value("column").toString()).arg(j).arg(i));

			if( !expr.contains("value") )
				return param_error("update action", "value", tr("where "
					"expression %1 in action %2").arg(j).arg(i));

			where_expr << QString("%1 %2 :where_%3").arg(column).arg(
				where_type).arg(column);
			where_values[column] = expr.value("value");
		}

		where = QString(" WHERE %1").arg( where_expr.join(where_connect_type) );
	}

	QString group_by;

	if( action.contains("group_by") )
	{
		QVariantList group_by_list = action.value("group_by").toList();

		if(group_by_list.count() < 1)
			return herror("update action", tr("paramater 'group_by' for action "
				"%1 exists but contains no group expressions").arg(i));

		QStringList group_by_expr;

		for(int j = 0; j < group_by_list.count(); j++)
		{
			QVariantMap group = group_by_list.at(j).toMap();

			if( !group.contains("column") )
				return param_error("update action", "column", tr("group "
					"expression %1 in action %2").arg(j).arg(i));

			// MySQL error for action 1: Column 'id' in
			// where clause is ambiguous
			// i.e. make the column name full (table.column)
			QString column = group.value("column").toString();
			// TODO: Check that this won't be an issue
			/*
			if( !relation_join_sql.isEmpty() ||
				relation_aliases.contains(column) )
					column = full_column_name(table, column);
			*/

			// Check all tables, including relation tables
			if( !check_column(column, table, db) &&
				!relation_aliases.contains( group.value("column").toString() ) )
					return herror("update action", tr("invalid column "
						"'%1' for group expression %2 in action %3").arg(
						group.value("column").toString()).arg(j).arg(i));

			QString direction = "ASC";

			if( group.contains("direction") )
			{
				direction = group.value("direction").toString().toUpper();

				if(direction != "ASC" && direction != "DESC")
					return herror("update action", tr("invalid direction "
						"'%1' for group expression %2 in action %3").arg(
						direction).arg(j).arg(i));
			}

			group_by_expr <<QString("%1 %2").arg(column).arg(direction);
		}

		group_by = QString(" GROUP BY %1").arg( group_by_expr.join(", ") );
	}

	QString order_by;

	if( action.contains("order_by") )
	{
		QVariantList order_by_list = action.value("order_by").toList();

		if(order_by_list.count() < 1)
			return herror("update action", tr("paramater 'order_by' for action "
				"%1 exists but contains no order expressions").arg(i));

		QStringList order_by_expr;

		for(int j = 0; j < order_by_list.count(); j++)
		{
			QVariantMap order = order_by_list.at(j).toMap();

			if( !order.contains("column") )
				return param_error("update action", "column", tr("order "
					"expression %1 in action %2").arg(j).arg(i));

			// MySQL error for action 1: Column 'id' in
			// where clause is ambiguous
			// i.e. make the column name full (table.column)
			QString column = order.value("column").toString();
			// TODO: Check that this won't be an issue
			/*
			if( !relation_join_sql.isEmpty() ||
				relation_aliases.contains(column) )
					column = full_column_name(table, column);
			*/

			// Check all tables, including relation tables
			if( !check_column(column, table, db) &&
				!relation_aliases.contains( order.value("column").toString() ) )
					return herror("update action", tr("invalid column "
						"'%1' for order expression %2 in action %3").arg(
						order.value("column").toString()).arg(j).arg(i));

			QString direction = "ASC";

			if( order.contains("direction") )
			{
				direction = order.value("direction").toString().toUpper();

				if(direction != "ASC" && direction != "DESC")
					return herror("update action", tr("invalid direction "
						"'%1' for order expression %2 in action %3").arg(
						direction).arg(j).arg(i));
			}

			order_by_expr <<QString("%1 %2").arg(column).arg(direction);
		}

		order_by = QString(" ORDER BY %1").arg( order_by_expr.join(", ") );
	}

	QString limit;

	if( action.contains("limit") )
	{
		int limit_value = action.value("limit").toInt();

		if(limit_value < 1)
			return herror("update action", tr("limit for in action %1 must be a "
				"positive integer").arg(i));

		if( action.contains("offset") )
		{
			QVariant offset = action.value("offset");
			int offset_value = limit.toInt();

			if(!offset.canConvert(QVariant::Int) || offset_value < 0)
				return herror("update action", tr("offset must be a positive "
					"integer for action %1").arg(i));

			limit = QString(" LIMIT %1, %2").arg(offset_value).arg(limit_value);
		}
		else
		{
			limit = QString(" LIMIT %1").arg(limit_value);
		}
	}
	else if( action.contains("offset") )
	{
		return herror("update action", "to include the paramater 'offset', "
			"you must also include the paramater 'limit'");
	}

	QSqlDatabase m_db = db;
	m_db.transaction();

	if( action.contains("relations") )
	{
		QVariantMap relations = action.value("relations").toMap();

		foreach(QString foreign_table, relations.keys())
		{
			QVariantMap relation = relations.value(foreign_table).toMap();

			QVariantMap columns_and_aliases = relation.value("columns").toMap();
			QStringList where_columns;

			// Prepare the bind names
			QMapIterator<QString, QVariant> it = columns_and_aliases;
			while( it.hasNext() )
			{
				it.next();

				QString alias = it.value().toString();
				if( alias.trimmed().isEmpty() )
					alias = QString("%1_%2").arg(foreign_table).arg(it.key());

				where_columns << QString("%1 = :%2").arg(it.key()).arg(alias);
			}

			QString foreign_id = relation.value("foreign_id").toString();
			QString sql = QString("SELECT %1 FROM %2 WHERE %3 LIMIT 1").arg(
				foreign_id).arg(foreign_table).arg(where_columns.join(" AND "));

			QSqlQuery query(db);
			query.prepare(sql);

			QString relation_id = relation.value("id").toString();

			for(int j = 0; j < row_count; j++)
			{
				it.toFront();
				while( it.hasNext() )
				{
					it.next();

					QString alias = it.value().toString();
					if( alias.trimmed().isEmpty() )
						alias = QString(":%1_%2").arg(foreign_table).arg(
							it.key());

					QVariant value = rows.at(j).toMap().value(alias);
					query.bindValue(QString(":%1").arg(alias), value);
				}

				if( !query.exec() )
					return herror_sql(db, "update action", tr("SQL error for "
						"relation '%1' in action %2: %3").arg(
						foreign_table).arg(i).arg( query.lastError().text() ));

				QVariant id;
				if( !query.next() )
				{
					query.record().value(foreign_id);
				}
				else
				{
					// If we are allowed to insert the relations, do so
					if( relation.contains("insert") && relation.value(
						"insert").toString().toLower() == "true")
					{
						QString where_columns_set = where_columns.join(", ");

						QString sql = QString("INSERT INTO %1 SET %2").arg(
							foreign_table).arg(where_columns_set);

						QSqlQuery query2(db);
						query2.prepare(sql);

						it.toFront();
						while( it.hasNext() )
						{
							it.next();

							QString alias = it.value().toString();
							if( alias.trimmed().isEmpty() )
								alias = QString(":%1_%2").arg(
									foreign_table).arg(it.key());

							QVariant value = rows.at(j).toMap().value(alias);
							query.bindValue(QString(":%1").arg(alias), value);
						}

						if( !query.exec() )
							return herror_sql(db, "update action", tr("SQL "
								"error for relation '%1' in action %2").arg(
								foreign_table).arg(i));

						id = query.lastInsertId();
					}
					else
					{
						return herror("update action", tr("unable to get id for"
							" relation '%1' in action %2").arg(
							foreign_table).arg(i));
					}
				}

				// Save the id to the row
				QVariantMap row = rows.at(j).toMap();
				row[relation_id] = id;

				// Remove all the relation rows
				it.toFront();
				while( it.hasNext() )
				{
					it.next();

					QString alias = it.value().toString();
					if( alias.trimmed().isEmpty() )
						alias = QString(":%1_%2").arg(
							foreign_table).arg(it.key());

					row.remove(alias);
				}

				// Save the row
				rows[j] = row;
			}
		}
	}

	QStringList value_holders;
	columns = rows.first().toMap().keys();

	foreach(QString column, columns)
		value_holders << QString("%1 = :%2").arg(column).arg(column);

	QString sql = QString("UPDATE %1 SET %2%3%4%5").arg(table).arg(
		value_holders.join(", ") ).arg(where).arg(order_by).arg(limit);

	QSqlQuery query(db);
	query.prepare(sql);

	QVariantList ids;
	for(int j = 0; j < row_count; j++)
	{
		QVariantMap row = rows.at(j).toMap();
		QMapIterator<QString, QVariant> it = row;

		while( it.hasNext() )
		{
			it.next();
			query.bindValue(QString(":%1").arg(it.key()), it.value());
		}

		it = where_values;
		while( it.hasNext() )
		{
			it.next();
			query.bindValue(QString(":where_%1").arg(it.key()), it.value());
		}

		if( !query.exec() )
			return herror_sql(db, "update action", tr("SQL error for row %1 in "
				"action %2: %3").arg(j).arg(j).arg( query.lastError().text() ));

		ids << query.lastInsertId();
	}

	m_db.commit();

	QVariantMap map;
	map["ids"] = ids;

	return map;
};

QVariantMap backendActionSelect(int i, QTcpSocket *connection,
	const QSqlDatabase& db, const QVariantMap& action, const QString& email)
{
	Q_UNUSED(email);
	Q_UNUSED(connection);

	// Check for the 'table' paramater
	if( !action.contains("table") )
		return param_error(tr("select action"), tr("table"),
			tr("action %1").arg(i));

	// To make life easier
	QString table = action.value("table").toString();

	if( tables_blacklist().contains(table) || !db.tables().contains(table) )
		return herror(tr("select action"), tr("table '%1' for action %2 does "
			"not exist").arg(table).arg(i));

	if( !action.contains("columns") )
		return param_error("select action", "columns", tr("action %1").arg(i));

	QStringList columns = action.value("columns").toStringList();

	if(columns.count() <= 0)
		return herror("select action", tr("paramater 'columns' for action %1 "
			"exists but contains no columns").arg(i));

	foreach(QString column, columns)
	{
		if( !check_column(column, table, db) )
			return herror("select action", tr("column '%1' for action %2 "
				"does not exist").arg(column).arg(i));
	}

	QString relation_join_sql;
	QStringList relation_tables;
	QStringList relation_columns;
	QStringList relation_aliases;

	// Check if there is any relations
	if( action.contains("relations") )
	{
		// Prefix the columns since we are doing a relation
		{
		QStringList new_columns;
		foreach(QString column, columns)
			new_columns << QString("%1.%2").arg(table).arg(column);

		columns = new_columns;
		}

		QVariantMap relations = action.value("relations").toMap();

		if(relations.count() < 1)
			return herror("select action", tr("paramater 'relations' for "
				"action %1 exists but contains no relations").arg(i));

		QStringList relation_ids;

		foreach(QString foreign_table, relations.keys())
		{
			QVariantMap relation = relations.value(foreign_table).toMap();

			if( tables_blacklist().contains(foreign_table) ||
				!db.tables().contains(foreign_table) )
					return herror("select action", tr("table '%1' for "
						"relation '%2' in action %3 does not exist").arg(
						foreign_table).arg(foreign_table).arg(i));

			if( !relation.contains("id") )
				return param_error("select action", "id", tr("relation "
					"'%1' in action %2").arg(foreign_table).arg(i));

			QString relation_id = relation.value("id").toString();

			if( !check_column(relation_id, foreign_table, db) )
				return herror("select action", tr("column '%1' on table '%2' "
				"for relation '%3' in action %4 does not exist").arg(
				relation_id).arg(table).arg(foreign_table).arg(i));

			if( !relation.contains("foreign_id") )
				return param_error("select action", "foreign_id", tr("relation "
					"'%1' in action %2").arg(foreign_table).arg(i));

			QString foreign_id = relation.value("foreign_id").toString();

			if( !check_column(foreign_id, foreign_table, db) )
				return herror("select action", tr("column '%1' on table '%2' "
				"for relation '%3' in action %3 does not exist").arg(
				foreign_id).arg(foreign_table).arg(foreign_table).arg(i));

			if( !relation.contains("columns") )
				return param_error("select action", "columns", tr("relation "
					"'%1' in action %2").arg(foreign_table).arg(i));

			QVariantMap columns_and_aliases = relation.value("columns").toMap();

			if(columns_and_aliases.count() <= 0)
				return herror("select action", tr("paramater 'columns' for "
					"relation '%1' in action %2 exists but contains no "
					"columns").arg(foreign_table).arg(i));

			foreach(QString relation_column, columns_and_aliases.keys())
			{
				QString alias = columns_and_aliases.value(
					relation_column).toString();

				if( !check_column(relation_column, foreign_table, db) )
					return herror("select action", tr("invalid column '%1' for "
					"relation '%2' in action %3").arg(
					relation_column).arg(foreign_table).arg(i));

				if( alias.trimmed().isEmpty() )
					alias = QString("%1_%2").arg(foreign_table).arg(
						relation_column);

				if(columns.contains(alias) || relation_aliases.contains(alias))
					return herror("select action", tr("alias '%1' of column"
 						"'%2' for relation '%3' in action %4 conflicts with an "
 						"existing column or alias").arg(alias).arg(
 						relation_column).arg(foreign_table).arg(i));

				relation_aliases << alias;
				relation_columns << QString("%1.%2 %3").arg(
					foreign_table).arg(relation_column).arg(alias);
			}

			relation_tables << foreign_table;
			relation_ids << QString("%1.%2 = %3.%4").arg(foreign_table).arg(
				foreign_id).arg(table).arg(relation_id);
		}

		relation_join_sql = QString(" LEFT JOIN (%1) ON (%2)").arg(
			relation_tables.join(", ") ).arg( relation_ids.join(" AND ") );
	}

	columns  << relation_columns;

	QString where;
	QVariantMap where_values;
	QString where_connect_type = " AND ";

	if( action.contains("where_type") )
	{
		where_connect_type = action.value("where_type").toString().toLower();

		if( !where_connect_types().contains(where_connect_type) )
			return herror("select action", tr("invalid where type "
				"'%1' for action %2").arg(where_connect_type).arg(i));

		where_connect_type = where_connect_types().value(where_connect_type);
	}

	if( action.contains("where") )
	{
		// TODO: More type-checking like this
		if( !action.value("where").canConvert(QVariant::List) )
			return herror("select action", tr("paramater 'where' for action %1 "
				"exists but is not an array").arg(i));

		QVariantList where_list = action.value("where").toList();

		if(where_list.count() < 1)
			return herror("select action", tr("paramater 'where' for action %1 "
				"exists but contains no where expressions").arg(i));

		QString where_type;
		QStringList where_expr;

		for(int j = 0; j < where_list.count(); j++)
		{
			QVariantMap expr = where_list.at(j).toMap();

			if( !expr.contains("type") )
				return param_error("select action", "type", tr("where "
					"expression %1 in action %2").arg(j).arg(i));

			// TODO: Make make it so this changes instead of just replacing
			// the old type with the new one
			where_type = expr.value("type").toString().toLower();

			if( !where_types().contains(where_type)  )
				return herror("select action", tr("where expression type %1 "
					"does not exists for where expression %2 in action "
					"%3").arg(where_type).arg(j).arg(i));

			where_type = where_types().value(where_type);

			if( !expr.contains("column") )
				return param_error("select action", "column", tr("where "
					"expression %1 in action %2").arg(j).arg(i));

			// MySQL error for action 1: Column 'id' in
			// where clause is ambiguous
			// i.e. make the column name full (table.column)
			QString column = expr.value("column").toString();
			if( !relation_join_sql.isEmpty() ||
				relation_aliases.contains(column) )
					column = full_column_name(table, column);

			// Check all tables, including relation tables
			if( !check_column(column, table, db) &&
				!relation_aliases.contains( expr.value("column").toString() ) )
					return herror("select action", tr("invalid column "
						"'%1' for where expression %2 in action %3").arg(
						expr.value("column").toString()).arg(j).arg(i));

			if( !expr.contains("value") )
				return param_error("select action", "value", tr("where "
					"expression %1 in action %2").arg(j).arg(i));

			where_expr << QString("%1 %2 :where_%3").arg(column).arg(
				where_type).arg(column);
			where_values[column] = expr.value("value");
		}

		where = QString(" WHERE %1").arg( where_expr.join(where_connect_type) );
	}

	QString group_by;

	if( action.contains("group_by") )
	{
		QVariantList group_by_list = action.value("group_by").toList();

		if(group_by_list.count() < 1)
			return herror("select action", tr("paramater 'group_by' for action "
				"%1 exists but contains no group expressions").arg(i));

		QStringList group_by_expr;

		for(int j = 0; j < group_by_list.count(); j++)
		{
			QVariantMap group = group_by_list.at(j).toMap();

			if( !group.contains("column") )
				return param_error("select action", "column", tr("group "
					"expression %1 in action %2").arg(j).arg(i));

			// MySQL error for action 1: Column 'id' in
			// where clause is ambiguous
			// i.e. make the column name full (table.column)
			QString column = group.value("column").toString();
			if( !relation_join_sql.isEmpty() ||
				relation_aliases.contains(column) )
					column = full_column_name(table, column);

			// Check all tables, including relation tables
			if( !check_column(column, table, db) &&
				!relation_aliases.contains( group.value("column").toString() ) )
					return herror("select action", tr("invalid column "
						"'%1' for group expression %2 in action %3").arg(
						group.value("column").toString()).arg(j).arg(i));

			QString direction = "ASC";

			if( group.contains("direction") )
			{
				direction = group.value("direction").toString().toUpper();

				if(direction != "ASC" && direction != "DESC")
					return herror("select action", tr("invalid direction "
						"'%1' for group expression %2 in action %3").arg(
						direction).arg(j).arg(i));
			}

			group_by_expr <<QString("%1 %2").arg(column).arg(direction);
		}

		group_by = QString(" GROUP BY %1").arg( group_by_expr.join(", ") );
	}

	QString order_by;

	if( action.contains("order_by") )
	{
		QVariantList order_by_list = action.value("order_by").toList();

		if(order_by_list.count() < 1)
			return herror("select action", tr("paramater 'order_by' for action "
				"%1 exists but contains no order expressions").arg(i));

		QStringList order_by_expr;

		for(int j = 0; j < order_by_list.count(); j++)
		{
			QVariantMap order = order_by_list.at(j).toMap();

			if( !order.contains("column") )
				return param_error("select action", "column", tr("order "
					"expression %1 in action %2").arg(j).arg(i));

			// MySQL error for action 1: Column 'id' in
			// where clause is ambiguous
			// i.e. make the column name full (table.column)
			QString column = order.value("column").toString();
			if( !relation_join_sql.isEmpty() ||
				relation_aliases.contains(column) )
					column = full_column_name(table, column);

			// Check all tables, including relation tables
			if( !check_column(column, table, db) &&
				!relation_aliases.contains( order.value("column").toString() ) )
					return herror("select action", tr("invalid column "
						"'%1' for order expression %2 in action %3").arg(
						order.value("column").toString()).arg(j).arg(i));

			QString direction = "ASC";

			if( order.contains("direction") )
			{
				direction = order.value("direction").toString().toUpper();

				if(direction != "ASC" && direction != "DESC")
					return herror("select action", tr("invalid direction "
						"'%1' for order expression %2 in action %3").arg(
						direction).arg(j).arg(i));
			}

			order_by_expr <<QString("%1 %2").arg(column).arg(direction);
		}

		order_by = QString(" ORDER BY %1").arg( order_by_expr.join(", ") );
	}

	QString limit;

	if( action.contains("limit") )
	{
		int limit_value = action.value("limit").toInt();

		if(limit_value < 1)
			return herror("select action", tr("limit for in action %1 must be a "
				"positive integer").arg(i));

		if( action.contains("offset") )
		{
			QVariant offset = action.value("offset");
			int offset_value = limit.toInt();

			if(!offset.canConvert(QVariant::Int) || offset_value < 0)
				return herror("select action", tr("offset must be a positive "
					"integer for action %1").arg(i));

			limit = QString(" LIMIT %1, %2").arg(offset_value).arg(limit_value);
		}
		else
		{
			limit = QString(" LIMIT %1").arg(limit_value);
		}
	}
	else if( action.contains("offset") )
	{
		return herror("select action", "to include the paramater 'offset', "
			"you must also include the paramater 'limit'");
	}

	QString sql = QString("SELECT %1 FROM %2%3%4%5%6%7").arg(
		columns.join(", ") ).arg(table).arg(relation_join_sql).arg(where).arg(
		group_by).arg(order_by).arg(limit);

	QSqlQuery query(db);
	if( !query.prepare(sql) )
		return herror_sql(db, "select action", tr("SQL error for action "
			"%1: %2").arg(i).arg( query.lastError().text() ));

	QMapIterator<QString, QVariant> it = where_values;
	while( it.hasNext() )
	{
		it.next();
		query.bindValue(QString(":where_%1").arg(it.key()), it.value());
	}

	if( !query.exec() )
		return herror_sql(db, "select action", tr("SQL error for action "
			"%1: %2").arg(i).arg( query.lastError().text() ));

	QVariantList rows;
	while( query.next() )
	{
		QSqlRecord record = query.record();

		QVariantMap row;
		for(int j = 0; j < record.count(); j++)
			row[record.fieldName(j)] = record.value(j);

		rows << row;
	}

	QVariantMap map;
	map["rows"] = rows;

	return map;
};
