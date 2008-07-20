<?php

/******************************************************************************\
*  website/backend.php                                                         *
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

setlocale(LC_ALL, 'en_US.UTF-8');

//header("content-type: text/html");
header("content-type: application/x-json");

require_once('lib/config.inc');
require_once('lib/auth.inc');

$json = $_POST['request'];
$json = stripslashes($json);

// Start the authentication module
auth_start();

// Authenticate the request
if( isset($_POST['email']) && !auth_authenticate($_POST['email'], $json, $_POST['pass']) )
{
	$err = "Authentication Error";

	header('X-JSON: (' . json_encode( array( array('error' => $err) ) ) . ')');

	exit(0);
}

$request = json_decode($json, true);

function transaction_started()
{
	global $db;

	try
	{
		if( $db->beginTransaction() )
			return true;

		$db-commit();

		return false;
	}
	catch(PDOException $e)
	{
		return true;
	}
}

function _array_trim(&$value)
{
    $value = trim($value);
}

function array_trim(&$arr)
{
    array_walk($arr, '_array_trim');
}

function _array_tolower(&$value)
{
    $value = mb_strtolower($value);
}

function array_tolower(&$arr)
{
    array_walk($arr, '_array_tolower');
}

function array_remove($value, &$arr)
{
	$key = array_search($value, $arr);
	if($key)
	{
		unset($arr[$key]);
		$arr = array_values($arr);
	}
}

function array_merge_unique($arr1, $arr2)
{
	return array_unique( array_merge($arr1, $arr2) );
}

$where_types['greater_then'] = '>';
$where_types['less_then']    = '<';
$where_types['equals']       = '=';

$where_connect_types['or'] = ' OR ';
$where_connect_types['and'] = ' AND ';

// Handle Error
function herror($type, $err)
{
	//print("Error parsing $type: $err\n");

	return array('error' => "Error parsing $type: $err");
}

function herror_sql($type, $err)
{
	global $db;

	if( transaction_started() )
		$db->rollBack();

	$db_err = $db->errorInfo();

	//print("Error parsing $type: $err: {$db_err[2]}\n");

	return array('error' => "Error parsing $type: $err: {$db_err[2]}");
}

function herror_stmt($stmt, $type, $err)
{
	global $db;

	if( transaction_started() )
		$db->rollBack();

	$stmt_err = $stmt->errorInfo();

	//print("Error parsing $type: $err: {$stmt_err[2]}\n");

	return array('error' => "Error parsing $type: $err: {$stmt_err[2]}");
}

function param_error($type, $param, $for)
{
	//print("Error parsing $type: paramater '$param' for $for is missing\n");

	return array('error' => "Error parsing $type: paramater '$param' for $for is missing");
}

$column_cache = array();
$tables_blacklist = array('users');

function db_error($err)
{
	global $db;

	$err = $db->errorInfo();
	$err = "MySQL Error: " . $err[2];

	//print("$err\n");
	return array('error' => $err);
}

function db_connect()
{
	global $db, $tables_blacklist, $DB;

	$dsn = "mysql:dbname={$DB['db']};host={$DB['host']};charset=UTF-8";

	try
	{
    	$db = new PDO($dsn, $DB['user'], $DB['pass']);
	}
	catch(PDOException $e)
	{
		$err = "MySQL Error: " . $e->getMessage();

		header('X-JSON: (' . json_encode( array( array('error' => $err) ) ) . ')');

		exit(0);
	}

	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
	$db->setAttribute(PDO::ATTR_CASE, PDO::CASE_NATURAL);
}

function cache_tables()
{
	global $DB, $db, $tables_blacklist, $table_cache;

	if( !isset($table_cache) )
	{
		$stmt = $db->query("SHOW TABLES", PDO::FETCH_ASSOC);
		if(!$stmt)
			return db_error();

		while( $row = $stmt->fetch() )
		{
			if( in_array($row["Tables_in_{$DB['db']}"], $tables_blacklist) )
				continue;

			$table_cache[] = $row["Tables_in_{$DB['db']}"];
		}

		$stmt->closeCursor();
	}

	return $table_cache;
}

function cache_table_status($table)
{
	global $db, $tables_blacklist, $table_status_cache;

	if( !isset($table_status_cache) )
	{
		$stmt = $db->query("SHOW TABLE STATUS", PDO::FETCH_ASSOC);
		if(!$stmt)
			return db_error();

		while( $row = $stmt->fetch() )
		{
			if( in_array($row['Name'], $tables_blacklist) )
				continue;

			$table_status_cache[ $row['Name'] ] = $row;
		}

		$stmt->closeCursor();
	}

	return $table_status_cache[$table];
}

function cache_columns($table)
{
	global $db, $column_cache;

	if( !isset($column_cache[$table]) )
	{
		$stmt = $db->query("DESCRIBE $table", PDO::FETCH_ASSOC);
		if(!$stmt)
			return db_error();

		while( $row = $stmt->fetch() )
			$column_cache[$table][] = $row['Field'];

		$stmt->closeCursor();
	}

	return $column_cache[$table];
}

function full_column_name($table, $column)
{
	if( mb_strpos($column, '.') )
		return $column;
	else
		return "{$table}.{$column}";
}

function check_column($col, $table, $column_cache)
{
	if( mb_strpos($col, '.') )
	{
		$col = explode('.', $col);
		if( !in_array($col[1], $column_cache[ $col[0] ]) )
			return false;
	}
	else
	{
		if($table == '')
			return false;

		if( !in_array($col, $column_cache[$table]) )
			return false;
	}

	return true;
}

function action_delete($i, $action)
{
	global $db, $where_types, $where_connect_types;

	// Cache the tables for table checks
	$tables = cache_tables();

	// Check for the 'table' paramater
	if( !array_key_exists('table', $action) )
		return param_error("delete action", "table", "action $i");

	// To make life easier
	$table = $action['table'];

	if( !in_array($table, $tables) )
		return herror("delete action", "table '{$action['table']}' for " .
			"action $i does not exist");

	$status = cache_table_status($table);

	// Check the table supports transactions
	if($status['Engine'] != 'InnoDB')
		return herror("delete action", "table '$table' for " .
			"action $i does not support transactions");

	$column_cache[$table] = cache_columns($table);

	$where = array();
	$where_values = array();
	$where_type = ' AND ';

	if( array_key_exists('where_type', $action) )
	{
		if( !array_key_exists($action['where_type'], $where_connect_types) )
			return herror("delete action", "invalid where type " .
				"'{$action['where_type']}' for action $i");

		$where_type = $where_connect_types[ $action['where_type'] ];
	}

	if( array_key_exists('where', $action) )
	{
		if( !is_array($action['where']) )
			return herror("delete action", "paramater 'where' for action $i " .
				"exists but is not an object");

		if( !count($action['where']) )
			return herror("delete action", "paramater 'where' for action $i " .
				"exists but contains no where expressions");

		$j = 0;
		foreach($action['where'] as $expr)
		{
			if( !array_key_exists('type', $expr) )
				return param_error("delete action", "type", "where " .
					"expression $j in action $i");

			if( !array_key_exists($expr['type'], $where_types) )
				return herror("delete action", "where expression type {$expr['type']}" .
					" does not exists for where expression $j in action $i");

			if( !array_key_exists('column', $expr) )
				return param_error("delete action", "column", "where " .
					"expression $j in action $i");

			$column = $expr['column'];

			// Check all tables, including relation tables
			if( !check_column($column, $table, $column_cache) )
					return herror("delete action", "invalid column " .
						"'$column' for where expression $j in action $i");

			if( !array_key_exists('value', $expr) )
				return param_error("delete action", "value", "where " .
					"expression $j in action $i");

			$where[] = "$column {$where_types[ $expr['type'] ]} :where_$column";
			$where_values[":where_$column"] = $expr['value'];
			$j++;
		}

		$where = " WHERE " . implode($where_type, $where);
	}
	else
	{
		$where = '';
	}

	$order_by = array();

	if( array_key_exists('order_by', $action) )
	{
		if( !is_array($action['order_by']) )
			return herror("delete action", "paramater 'order_by' for action $i " .
				"exists but is not an object");

		if( !count($action['order_by']) )
			return herror("delete action", "paramater 'order_by' for action $i " .
				"exists but contains no order expressions");

		$j = 0;
		foreach($action['order_by'] as $order)
		{
			if( !array_key_exists('column', $order) )
				return param_error("delete action", "column", "order " .
					"expression $j in action $i");

			$column = $order['column'];

			// Check all tables, including relation tables
			if( !check_column($column, $table, $column_cache) )
				return herror("delete action", "invalid column " .
					"'$column' for order expression $j in action $i");

			$direction = 'ASC';

			if( array_key_exists('direction', $order) )
			{
				$direction = mb_strtoupper($order['direction']);

				if($direction != 'ASC' && $direction != 'DESC')
					return herror("delete action", "invalid direction " .
						"'$direction' for order expression $j in action $i");
			}

			$order_by[] = "$column $direction";
			$j++;
		}

		$order_by = " ORDER BY " . implode(', ', $order_by);
	}
	else
	{
		$order_by = '';
	}

	$limit = '';
	if( array_key_exists('limit', $action) )
	{
		if(!is_int($action['limit']) || $action['limit'] < 0)
			return herror("delete action", "limit for in action $i must be a " .
				"positive integer");

		$limit = " LIMIT {$action['limit']}";
	}

	if( !auth_validate_request($_POST['email'], $action) )
		return herror("delete action", "authentication error for action $i");

	// Only commit the changes if there are no errors
	if( !$db->beginTransaction() )
		herror_sql("delete action", "failed to start transaction for action $i");

	$sql = "DELETE FROM ${table}{$where}{$order_by}{$limit}";

	$stmt = $db->prepare($sql);
	if(!$stmt)
		return herror_sql("delete action", "MySQL error for row action $i");

	if( !$stmt->execute($where_values) )
	{
		$err = $stmt->errorInfo();
		return herror_stmt($stmt, "delete action", "MySQL error in action " .
			"$i: " . $err[2]);
	}

	$stmt->closeCursor();

	$db->commit();
}

function action_insert($i, $action)
{
	global $db;

	// Cache the tables for table checks
	$tables = cache_tables();

	// Check for the 'table' paramater
	if( !array_key_exists('table', $action) )
		return param_error("insert action", "table", "action $i");

	// To make life easier
	$table = $action['table'];

	if( !in_array($table, $tables) )
		return herror("insert action", "table '{$action['table']}' for " .
			"action $i does not exist");

	$status = cache_table_status($table);

	// Check the table supports transactions
	if($status['Engine'] != 'InnoDB')
		return herror("insert action", "table '$table' for " .
			"action $i does not support transactions");

	// Check for the 'rows' paramater
	if( !array_key_exists('rows', $action) )
		return param_error("insert action", "rows", "action $i");

	// Check the 'rows' paramater is an array
	if( !is_array($action['rows']) )
		return herror("insert action", "parmater 'rows' for action $i is not an array");

	// Check the 'rows' paramater is not empty
	if( !count($action['rows']) )
		return herror("insert action", "no rows to insert for action $i");

	$column_cache[$table] = cache_columns($table);

	$relation_aliases = array();

	if( array_key_exists('relations', $action) )
	{
		if( !is_array($action['relations']) )
			return herror("insert action", "parmater 'relations' for action $i " .
				"is not an object");

		if( !count($action['relations']) )
			return herror("select action", "paramater 'relations' for action $i " .
				"exists but contains no relations");

		foreach($action['relations'] as $foreign_table => $relation)
		{
			if( !in_array($foreign_table, $tables) )
				return herror("insert action", "table '$foreign_table' for " .
					"relation '$foreign_table' in action $i does not exist");

			if( !array_key_exists('id', $relation) )
				return param_error("insert action", "id", "relation " .
					"'$foreign_table' in action $i");

			if( !in_array($relation['id'], $column_cache[$table]) )
				return herror("insert action", "column '{$relation['id']}' " .
					"on table '$table' for relation '$foreign_table' in " .
					"action $i does not exist");

			if( !array_key_exists('foreign_id', $relation) )
				return param_error("insert action", "foreign_id", "relation " .
					"'$foreign_table' in action $i");

			$column_cache[$foreign_table] = cache_columns($foreign_table);
			if( !in_array($relation['foreign_id'], $column_cache[$foreign_table]) )
				return herror("insert action", "column '{$relation['foreign_id']}' " .
					"on table '$foreign_table' for relation '$foreign_table' in " .
					"action $i does not exist");

			foreach($relation['columns'] as $relation_column => $alias)
			{
				if( in_array($alias, $column_cache[$table])
					|| array_key_exists($alias, $relation_aliases) )
						return herror("insert action", "alias '$alias' of column" .
	 						"'$relation_column' for relation '$foreign_table' in " .
							"action $i conflicts with an existing column or alias");

				$relation_aliases[] = $alias;
			}
		}
	}

	$columns = array();
	foreach($action['rows'] as $row)
		$columns = array_merge_unique( $columns, array_keys($row) );

	// Strip all columns that are for aliases
	$columns = array_values( array_diff($columns, $relation_aliases) );

	// Check all columns
	foreach($columns as $column)
	{
		if( !in_array($column, $column_cache[$table]) )
			return herror("insert action", "column '$column' for  " .
				"action $i does not exist");
	}

	$rows = $action['rows'];
	$row_count = count($rows);

	// Check all the columns are there
	for($j = 0; $j < $row_count; $j++)
	{
		$row_columns = array_keys($rows[$j]);

		$missing = array_values( array_diff($relation_aliases, $row_columns) );
		if( count($missing) )
			return herror("insert action", "column '{$missing[0]}' for row $j in " .
				"action $i does not exist");

		$missing = array_values( array_diff($columns, $row_columns) );
		if( count($missing) )
			return herror("insert action", "column '{$missing[0]}' for row $j in " .
				"action $i does not exist");
	}

	if( !auth_validate_request($_POST['email'], $action) )
		return herror("insert action", "authentication error for action $i");

	// Only commit the changes if there are no errors
	if( !$db->beginTransaction() )
		herror_sql("insert action", "failed to start transaction for action $i");

	if( array_key_exists('relations', $action) )
	{
		foreach($action['relations'] as $foreign_table => $relation)
		{
			$where_columns = array();

			// Prepare the bind names
			foreach($relation['columns'] as $relation_column => $alias)
				$where_columns[] = "$relation_column = :$alias";

			$where_columns_expr = implode(' AND ', $where_columns);

			$sql = "SELECT {$relation['foreign_id']} FROM $foreign_table WHERE " .
				"{$where_columns_expr} LIMIT 1";

			$stmt = $db->prepare($sql);
			if(!$stmt)
				return herror_stmt($stmt, "insert action", "MySQL error for " .
					"relation '$foreign_table' in action $i");

			for($j = 0; $j < $row_count; $j++)
			{
				$where_values = array();
				foreach($relation['columns'] as $relation_column => $alias)
					$where_values[$alias] = $rows[$j][$alias];

				if( !$stmt->execute($where_values) )
					return herror_stmt($stmt, "insert action", "MySQL error for " .
						"relation '$foreign_table' in action $i");

				if( $id = $stmt->fetchColumn() )
				{
					$stmt->closeCursor();

					$rows[$j][ $relation['id'] ] = $id;
				}
				else
				{
					$stmt->closeCursoe();

					// If we are allowed to insert the relations, do so
					if( array_key_exists('insert', $relation) &&
						mb_strtolower($relation['insert']) == 'true' )
					{
						$where_columns_set = implode(', ', $where_columns);

						$sql = "INSERT INTO $foreign_table SET {$where_columns_set}";

						$stmt = $db->prepare($sql);
						if(!$stmt)
							return herror_stmt($stmt, "insert action", "MySQL " .
								"error for relation '$foreign_table' in action $i");

						if( !$stmt->execute($where_values) )
							return herror_stmt($stmt, "insert action", "MySQL " .
								"error for relation '$foreign_table' in action $i");

						$rows[$j][ $relation['id'] ] = $db->lastInsertId();
					}
					else
					{
						return herror("insert action", "unable to get id for relation" . 							"'$foreign_table' in action $i");
					}
				}

				// Remove all the relation rows
				foreach($relation['columns'] as $relation_column => $alias)
					unset($rows[$j][$alias]);
			}
		}
	}

	$value_holders = array();
	$columns = array_keys($rows[0]);

	foreach($columns as $alias)
		$value_holders[] = ":$alias";

	$columns = implode(', ', $columns);
	$value_holders = implode(', ', $value_holders);

	$sql = "INSERT INTO $table ($columns) VALUES ($value_holders)";

	$stmt = $db->prepare($sql);
	if(!$stmt)
		return herror_sql("insert action", "MySQL error for row action $i");

	for($j = 0; $j < $row_count; $j++)
	{
		if( !$stmt->execute($rows[$j]) )
		{
			$err = $stmt->errorInfo();
			return herror_stmt($stmt, "insert action", "MySQL error for row " .
				"$j in action $i: " . $err[2]);
		}

		$stmt->closeCursor();
	}

	$db->commit();
}

function action_update($i, $action)
{
	global $db, $where_types, $where_connect_types;

	// Cache the tables for table checks
	$tables = cache_tables();

	// Check for the 'table' paramater
	if( !array_key_exists('table', $action) )
		return param_error("update action", "table", "action $i");

	// To make life easier
	$table = $action['table'];

	if( !in_array($table, $tables) )
		return herror("update action", "table '$table' for " .
			"action $i does not exist");

	$status = cache_table_status($table);

	// Check the table supports transactions
	if($status['Engine'] != 'InnoDB')
		return herror("update action", "table '$table' for " .
			"action $i does not support transactions");

	// Check for the 'rows' paramater
	if( !array_key_exists('rows', $action) )
		return param_error("update action", "rows", "action $i");

	// Check for the 'rows' paramater
	if( !count($action['rows']) )
		return herror("update action", "no rows to update for action $i");

	$column_cache[$table] = cache_columns($table);

	$relation_aliases = array();

	if( array_key_exists('relations', $action) )
	{
		if( !count($action['relations']) )
			return herror("update action", "paramater 'relations' for action $i " .
				"exists but contains no relations");

		foreach($action['relations'] as $foreign_table => $relation)
		{
			if( !in_array($foreign_table, $tables) )
				return herror("update action", "table '$foreign_table' for " .
					"relation '$foreign_table' in action $i does not exist");

			if( !array_key_exists('id', $relation) )
				return param_error("update action", "id", "relation " .
					"'$foreign_table' in action $i");

			if( !in_array($relation['id'], $column_cache[$table]) )
				return herror("update action", "column '{$relation['id']}' " .
					"on table '$table' for relation '$foreign_table' in " .
					"action $i does not exist");

			if( !array_key_exists('foreign_id', $relation) )
				return param_error("update action", "foreign_id", "relation " .
					"'$foreign_table' in action $i");

			$column_cache[$foreign_table] = cache_columns($foreign_table);
			if( !in_array($relation['foreign_id'], $column_cache[$foreign_table]) )
				return herror("update action", "column '{$relation['foreign_id']}' " .
					"on table '$foreign_table' for relation '$foreign_table' in " .
					"action $i does not exist");

			foreach($relation['columns'] as $relation_column => $alias)
			{
				if( in_array($alias, $column_cache[$table])
					|| array_key_exists($alias, $relation_aliases) )
						return herror("update action", "alias '$alias' of column" .
	 						"'$relation_column' for relation '$foreign_table' in " .
							"action $i conflicts with an existing column or alias");

				$relation_aliases[] = $alias;
			}
		}
	}

	$columns = array();
	foreach($action['rows'] as $row)
		$columns = array_merge_unique( $columns, array_keys($row) );

	// Strip all columns that are for aliases
	$columns = array_values( array_diff($columns, $relation_aliases) );

	// Check all columns
	foreach($columns as $column)
	{
		if( !in_array($column, $column_cache[$table]) )
			return herror("update action", "column '$column' for  " .
				"action $i does not exist");
	}

	$rows = $action['rows'];
	$row_count = count($rows);

	// Check all the columns are there
	for($j = 0; $j < $row_count; $j++)
	{
		$row_columns = array_keys($rows[$j]);

		$missing = array_values( array_diff($relation_aliases, $row_columns) );
		if( count($missing) )
			return herror("update action", "column '{$missing[0]}' for row $j in " .
				"action $i does not exist");

		$missing = array_values( array_diff($columns, $row_columns) );
		if( count($missing) )
			return herror("update action", "column '{$missing[0]}' for row $j in " .
				"action $i does not exist");
	}

	if( !auth_validate_request($_POST['email'], $action) )
		return herror("update action", "authentication error for action $i");

	// Only commit the changes if there are no errors
	if( !$db->beginTransaction() )
		herror_sql("update action", "failed to start transaction for action $i");

	if( array_key_exists('relations', $action) )
	{
		foreach($action['relations'] as $foreign_table => $relation)
		{
			$where_columns = array();

			// Prepare the bind names
			foreach($relation['columns'] as $relation_column => $alias)
				$where_columns[] = "$relation_column = :$alias";

			$where_columns_expr = implode(' AND ', $where_columns);

			$sql = "SELECT {$relation['foreign_id']} FROM $foreign_table WHERE " .
				"{$where_columns_expr} LIMIT 1";

			$stmt = $db->prepare($sql);
			if(!$stmt)
				return herror_stmt($stmt, "update action", "MySQL error for " .
					"relation '$foreign_table' in action $i");

			for($j = 0; $j < $row_count; $j++)
			{
				$where_values = array();
				foreach($relation['columns'] as $relation_column => $alias)
					$where_values[$alias] = $rows[$j][$alias];

				if( !$stmt->execute($where_values) )
					return herror_stmt($stmt, "update action", "MySQL error for " .
						"relation '$foreign_table' in action $i");

				if( $id = $stmt->fetchColumn() )
				{
					$stmt->closeCursor();

					$rows[$j][ $relation['id'] ] = $id;
				}
				else
				{
					$stmt->closeCursor();

					// If we are allowed to insert the relations, do so
					if( array_key_exists('insert', $relation) &&
						mb_strtolower($relation['insert']) == 'true' )
					{
						$where_columns_set = implode(', ', $where_columns);

						$sql = "INSERT INTO $foreign_table SET {$where_columns_set}";

						$stmt = $db->prepare($sql);
						if(!$stmt)
							return herror_stmt($stmt, "update action", "MySQL " .
								"error for relation '$foreign_table' in action $i");

						if( !$stmt->execute($where_values) )
							return herror_stmt($stmt, "update action", "MySQL " .
								"error for relation '$foreign_table' in action $i");

						$rows[$j][ $relation['id'] ] = $db->lastInsertId();
					}
					else
					{
						return herror("update action", "unable to get id for relation" .
								"'$foreign_table' in action $i");
					}
				}

				// Remove all the relation rows
				foreach($relation['columns'] as $relation_column => $alias)
					unset($rows[$j][$alias]);
			}
		}
	}

	$columns = array_keys($rows[0]);

	// WHERE, ORDER BY, LIMIT
	$where = array();
	$where_values = array();
	$where_type = ' AND ';

	if( array_key_exists('where_type', $action) )
	{
		if( !array_key_exists($action['where_type'], $where_connect_types) )
			return herror("update action", "invalid where type " .
				"'{$action['where_type']}' for action $i");

		$where_type = $where_connect_types[ $action['where_type'] ];
	}

	if( array_key_exists('where', $action) )
	{
		if( !count($action['where']) )
			return herror("update action", "paramater 'where' for action $i " .
				"exists but contains no where expressions");

		$j = 0;
		foreach($action['where'] as $expr)
		{
			if( !array_key_exists('type', $expr) )
				return param_error("update action", "type", "where " .
					"expression $j in action $i");

			if( !array_key_exists($expr['type'], $where_types) )
				return herror("update action", "where expression type {$expr['type']}" .
					" does not exists for where expression $j in action $i");

			if( !array_key_exists('column', $expr) )
				return param_error("update action", "column", "where " .
					"expression $j in action $i");

			$column = $expr['column'];

			// Check all tables, including relation tables
			if( !check_column($column, $table, $column_cache) )
					return herror("update action", "invalid column " .
						"'$column' for where expression $j in action $i");

			if( !array_key_exists('value', $expr) )
				return param_error("update action", "value", "where " .
					"expression $j in action $i");

			$where[] = "$column {$where_types[ $expr['type'] ]} :where_$column";
			$where_values[":where_$column"] = $expr['value'];
			$j++;
		}

		$where = " WHERE " . implode($where_type, $where);
	}
	else
	{
		$where = '';
	}

	$order_by = array();

	if( array_key_exists('order_by', $action) )
	{
		if( !count($action['order_by']) )
			return herror("update action", "paramater 'order_by' for action $i " .
				"exists but contains no order expressions");

		$j = 0;
		foreach($action['order_by'] as $order)
		{
			if( !array_key_exists('column', $order) )
				return param_error("update action", "column", "order " .
					"expression $j in action $i");

			$column = $order['column'];

			// Check all tables, including relation tables
			if( !check_column($column, $table, $column_cache) )
				return herror("update action", "invalid column " .
					"'$column' for order expression $j in action $i");

			$direction = 'ASC';

			if( array_key_exists('direction', $order) )
			{
				$direction = mb_strtoupper($order['direction']);

				if($direction != 'ASC' && $direction != 'DESC')
					return herror("update action", "invalid direction " .
						"'$direction' for order expression $j in action $i");
			}

			$order_by[] = "$column $direction";
			$j++;
		}

		$order_by = " ORDER BY " . implode(', ', $order_by);
	}
	else
	{
		$order_by = '';
	}

	$limit = '';
	if( array_key_exists('limit', $action) )
	{
		if(!is_int($action['limit']) || $action['limit'] < 0)
			return herror("update action", "limit for in action $i must be a " .
				"positive integer");

		$limit = " LIMIT {$action['limit']}";
	}

	$value_holders = array();

	foreach($columns as $alias)
		$value_holders[] = "$alias = :$alias";

	$value_holders = implode(', ', $value_holders);

	$sql = "UPDATE $table SET {$value_holders}{$where}{$order_by}{$limit}";

	$stmt = $db->prepare($sql);
	if(!$stmt)
		return herror_sql("insert action", "MySQL error for row action $i");

	for($j = 0; $j < $row_count; $j++)
	{
		if( !$stmt->execute( array_merge($rows[$j], $where_values) ) )
		{
			$err = $stmt->errorInfo();
			return herror_stmt($stmt, "insert action", "MySQL error for row " .
				"$j in action $i: " . $err[2]);
		}

		$stmt->closeCursor();
	}

	$db->commit();
}

function action_select($i, $action)
{
	global $db, $where_types, $where_connect_types;

	// Cache the tables for table checks
	$tables = cache_tables();

	// Check for the 'table' paramater
	if( !array_key_exists('table', $action) )
		return param_error("select action", "table", "action $i");

	// To make life easier
	$table = $action['table'];

	if( !in_array($table, $tables) )
		return herror("select action", "table '{$action['table']}' for " .
			"action $i does not exist");

	if( !array_key_exists('columns', $action) )
		return param_error("select action", "columns", "action $i");

	if( !count($action['columns']) )
		return herror("select action", "paramater 'columns' for action $i " .
			"exists but contains no columns");

	$column_cache[$table] = cache_columns($table);

	foreach($action['columns'] as $column)
	{
		if( !check_column($column, $table, $column_cache) )
			return herror("select action", "column '$column' for action $i does " .
				"not exist");
	}

	$columns = $action['columns'];

	$relation_tables = array();
	$relation_aliases = array();
	$relation_columns = array();
	$relation_join_sql = '';

	// Check if there is any relations
	if( array_key_exists('relations', $action) )
	{
		// Prefix the columns since we are doing a relation
		foreach($columns as $column)
			$new_columns[] = "{$table}.{$column}";

		$columns = $new_columns;

		if( !count($action['relations']) )
			return herror("select action", "paramater 'relations' for action $i " .
				"exists but contains no relations");

		$relation_ids = array();

		foreach($action['relations'] as $foreign_table => $relation)
		{
			if( !in_array($foreign_table, $tables) )
				return herror("select action", "table '$foreign_table' for " .
					"relation '$foreign_table' in action $i does not exist");

			if( !array_key_exists('id', $relation) )
				return param_error("select action", "id", "relation " .
					"'$foreign_table' in action $i");

			if( !in_array($relation['id'], $column_cache[$table]) )
				return herror("select action", "column '{$relation['id']}' " .
					"on table '$table' for relation '$foreign_table' in " .
					"action $i does not exist");

			if( !array_key_exists('foreign_id', $relation) )
				return param_error("select action", "foreign_id", "relation " .
					"'$foreign_table' in action $i");

			$column_cache[$foreign_table] = cache_columns($foreign_table);
			if( !in_array($relation['foreign_id'], $column_cache[$foreign_table]) )
				return herror("select action", "column '{$relation['foreign_id']}' " .
					"on table '$foreign_table' for relation '$foreign_table' in " .
					"action $i does not exist");

			if( !array_key_exists('columns', $relation) )
				return param_error("select action", "columns", "relation " .
					"'$foreign_table' in action $i");

			if( !count($relation['columns']) )
				return herror("select action", "paramater 'columns' for " .
					"relation '$foreign_table' in action $i exists but " .
					"contains no columns");

			foreach($relation['columns'] as $relation_column => $alias)
			{
				if( !check_column($relation_column, $foreign_table, $column_cache) )
					return herror("select action", "invalid column " .
						"'$relation_column' for relation '$foreign_table' in action $i");

				if( trim($alias) == '' )
					$alias = "{$foreign_table}_$relation_column";

				if( in_array($alias, $columns) || in_array($alias, $relation_aliases) )
					return herror("select action", "alias '$alias' of column" .
 						"'$relation_column' for relation '$foreign_table' in " .
						"action $i conflicts with an existing column or alias");

				$relation_aliases[] = $alias;
				$relation_columns[] = "{$foreign_table}.{$relation_column} $alias";
			}

			$relation_tables[] = $foreign_table;
			$relation_ids[] = "{$foreign_table}.{$relation['foreign_id']} = " .
				"{$action['table']}.{$relation['id']}";
		}

		$relation_join_sql = " LEFT JOIN (" . implode(', ', $relation_tables) .
			") ON (" . implode(' AND ', $relation_ids) . ")";
	}

	$columns = array_merge($columns, $relation_columns);

	$where = array();
	$where_values = array();
	$where_type = ' AND ';

	if( array_key_exists('where_type', $action) )
	{
		if( !array_key_exists($action['where_type'], $where_connect_types) )
			return herror("select action", "invalid where type " .
				"'{$action['where_type']}' for action $i");

		$where_type = $where_connect_types[ $action['where_type'] ];
	}

	if( array_key_exists('where', $action) )
	{
		if( !count($action['where']) )
			return herror("select action", "paramater 'where' for action $i " .
				"exists but contains no where expressions");

		$j = 0;
		foreach($action['where'] as $expr)
		{
			if( !array_key_exists('type', $expr) )
				return param_error("select action", "type", "where " .
					"expression $j in action $i");

			if( !array_key_exists($expr['type'], $where_types)  )
				return herror("select action", "where expression type {$expr['type']}" .
					" does not exists for where expression $j in action $i");

			if( !array_key_exists('column', $expr) )
				return param_error("select action", "column", "where " .
					"expression $j in action $i");

			// MySQL error for action 1: Column 'id' in where clause is ambiguous
			if($relation_join_sql == '' ||
				in_array($group['column'], $relation_aliases) )
					$column = $expr['column'];
			else
				$column = full_column_name($table, $expr['column']);

			// Check all tables, including relation tables
			if( !check_column($column, $table, $column_cache) &&
				!in_array($expr['column'], $relation_aliases) )
					return herror("select action", "invalid column " .
						"'$column' for where expression $j in action $i");

			if( !array_key_exists('value', $expr) )
				return param_error("select action", "value", "where " .
					"expression $j in action $i");

			$where[] = "$column {$where_types[ $expr['type'] ]} ?";
			$where_values[] = $expr['value'];
			$j++;
		}

		$where = " WHERE " . implode($where_type, $where);
	}
	else
	{
		$where = '';
	}

	$group_by = array();

	if( array_key_exists('group_by', $action) )
	{
		if( !count($action['group_by']) )
			return herror("select action", "paramater 'group_by' for action $i " .
				"exists but contains no group expressions");

		$j = 0;
		foreach($action['group_by'] as $group)
		{
			if( !array_key_exists('column', $group) )
				return param_error("select action", "column", "group " .
					"expression $j in action $i");

			// MySQL error for action 1: Column 'id' in where clause is ambiguous
			if($relation_join_sql == '' ||
				in_array($group['column'], $relation_aliases) )
					$column = $group['column'];
			else
				$column = full_column_name($table, $group['column']);

			// Check all tables, including relation tables
			if( !check_column($column, $table, $column_cache) &&
				!in_array($group['column'], $relation_aliases) )
					return herror("select action", "invalid column " .
						"'$column' for group expression $j in action $i");

			$direction = 'ASC';

			if( array_key_exists('direction', $group) )
			{
				$direction = mb_strtoupper($group['direction']);

				if($direction != 'ASC' && $direction != 'DESC')
					return herror("select action", "invalid direction " .
						"'$direction' for group expression $j in action $i");
			}

			$group_by[] = "$column $direction";
			$j++;
		}

		$group_by = " GROUP BY " . implode(', ', $group_by);
	}
	else
	{
		$group_by = '';
	}

	$order_by = array();

	if( array_key_exists('order_by', $action) )
	{
		if( !count($action['order_by']) )
			return herror("select action", "paramater 'order_by' for action $i " .
				"exists but contains no order expressions");

		$j = 0;
		foreach($action['order_by'] as $order)
		{
			if( !array_key_exists('column', $order) )
				return param_error("select action", "column", "order " .
					"expression $j in action $i");

			// MySQL error for action 1: Column 'id' in where clause is ambiguous
			if($relation_join_sql == '' ||
				in_array($group['column'], $relation_aliases) )
					$column = $order['column'];
			else
				$column = full_column_name($table, $order['column']);

			// Check all tables, including relation tables
			if( !check_column($column, $table, $column_cache) &&
				!in_array($order['column'], $relation_aliases) )
					return herror("select action", "invalid column " .
						"'$column' for order expression $j in action $i");

			$direction = 'ASC';

			if( array_key_exists('direction', $order) )
			{
				$direction = mb_strtoupper($order['direction']);

				if($direction != 'ASC' && $direction != 'DESC')
					return herror("select action", "invalid direction " .
						"'$direction' for order expression $j in action $i");
			}

			$order_by[] = "$column $direction";
			$j++;
		}

		$order_by = " ORDER BY " . implode(', ', $order_by);
	}
	else
	{
		$order_by = '';
	}

	$limit = '';
	if( array_key_exists('limit', $action) )
	{
		if(!is_int($action['limit']) || $action['limit'] < 0)
			return herror("select action", "limit for in action $i must be a " .
				"positive integer");

		if( array_key_exists('offset', $action) )
		{
			if(!is_int($action['offset']) || $action['offset'] < 0)
				return herror("select action", "offset must be a positive " .
					"integer for action $i\n");

			$limit = " LIMIT {$action['offset']}, {$action['limit']}";
		}
		else
		{
			$limit = " LIMIT {$action['limit']}";
		}
	}
	else if( array_key_exists('offset', $action) )
	{
		return herror("select action", "to include the paramater 'offset', " .
			"you must also include the paramater 'limit'");
	}

	if( !auth_validate_request($_POST['email'], $action) )
		return herror("select action", "authentication error for action $i");

	$columns = implode(', ', $columns);
	$sql = "SELECT $columns FROM {$action['table']}{$relation_join_sql}{$where}" .
		"{$group_by}{$order_by}{$limit}";

	$stmt = $db->prepare($sql);
	if(!$stmt)
		return herror_sql("select action", "MySQL error for action $i");

	if( !$stmt->execute($where_values) )
		return herror_stmt($stmt, "select action", "MySQL error for action $i");

	$rows = array();
	while( $row = $stmt->fetch(PDO::FETCH_ASSOC) )
		$rows[] = $row;

	$stmt->closeCursor();

	return array('rows' => $rows);
}

function action_salt($i, $action)
{
	global $MEGA_GLOBALS;

	return array('salt' => $MEGA_GLOBALS['PASS_SALT']);
}

function action_auth_query_perms($i, $action)
{
	if( isset($action['email']) )
	{
		return array('perms' => auth_query_perms($action['email']),
			'email' => $action['email']);
	}
	else
	{
		return array('perms' => auth_query_perms($_POST['email']),
			'email' => $_POST['email']);
	}
}

function action_auth_query_users($i, $action)
{
	return array('users' => auth_query_users($_POST['email']));
}

function action_auth_query_user($i, $action)
{
	// TODO: Error checking
	return array('user' => auth_query_user($_POST['email'], $action['email']));
}

function action_auth_make_inactive($i, $action)
{
	// TODO: Add some error checking
	auth_make_inactive($_POST['email'], $action['email']);

	return array();
}

function action_auth_make_active($i, $action)
{
	// TODO: Add some error checking
	auth_make_active($_POST['email'], $action['email']);

	return array();
}

function action_auth_modify_user($i, $action)
{
	// TODO: Add some error checking
	auth_modify_user($_POST['email'], $action['email'], $action['new_email'],
		$action['name'], $action['pass'], $action['perms']);

	return array();
}

$action_handlers['delete'] = action_delete;
$action_handlers['insert'] = action_insert;
$action_handlers['update'] = action_update;
$action_handlers['select'] = action_select;
$action_handlers['salt'] = action_salt;
$action_handlers['auth_query_perms'] = action_auth_query_perms;
$action_handlers['auth_query_users'] = action_auth_query_users;
$action_handlers['auth_query_user'] = action_auth_query_user;
$action_handlers['auth_make_inactive'] = action_auth_make_inactive;
$action_handlers['auth_make_active'] = action_auth_make_active;
$action_handlers['auth_modify_user'] = action_auth_modify_user;

db_connect();

$request_results = array();

if( !is_array($request) || !array_key_exists('actions', $request) )
{
	if( !is_array($request) )
		$request_results[] = herror("request", "Request is not an object");
	else
		$request_results[] = herror("request", "Request does not contain any actions");
}
else
{
	$i = 0;
	foreach($request['actions'] as $action)
	{
		if( !is_array($action) )
		{
			$request_results[] = herror("action", "action $i is not an object");
			continue;
		}

		if( !array_key_exists('action', $action) )
		{
			$request_results[] = herror("action", "action $i has no type");
			continue;
		}

		if( array_key_exists($action['action'], $action_handlers) )
		{
			$request_result = $action_handlers[ $action['action'] ]($i, $action);

			if( array_key_exists('user_data', $action) )
				$request_result['user_data'] = $action['user_data'];

			$request_result['action'] = $action['action'];

			$request_results[] = $request_result;
		}
		else
		{
			$request_results[] = herror("action",
				"unknown action type {$action['action']} for action $i");

			continue;
		}

		$i++;
	}
}

// Cleanup the auth module
auth_cleanup();

unset($db);

/*
print("<pre>");
print_r($request_results);
print("</pre>\n");
*/

// for($i = 0; $row = $stmt->fetch(); $i++)

header('X-JSON: (' . json_encode($request_results) . ')');

//print_r($request_results);
//echo json_encode($request_results);

?>
