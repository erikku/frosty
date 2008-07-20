<?php

/******************************************************************************\
*  website/lib/auth.inc                                                        *
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

require_once('config.inc');

// $DEFAULT_PERMS

function auth_start()
{
	global $auth_db, $DB;

	$dsn = "mysql:dbname={$DB['db']};host={$DB['host']};charset=UTF-8";

	try
	{
    	$auth_db = new PDO($dsn, $DB['user'], $DB['pass']);
	}
	catch(PDOException $e)
	{
		// Let's leave this cryptic so the user doesn't know it's an error
		// while connecting with the auth account

		$err = "MySQL Error";

		header('X-JSON: (' . json_encode( array( array('error' => $err) ) ) . ')');

		echo "Request OK";

		exit(0);
	}

	$auth_db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
	$auth_db->setAttribute(PDO::ATTR_CASE, PDO::CASE_NATURAL);
}

function auth_cleanup()
{
	global $auth_db;

	unset($auth_db);
}

function auth_authenticate($email, $data, $hash)
{
	global $auth_db;

	$sql = "SELECT pass, active FROM users WHERE email = :email";

	$stmt = $auth_db->prepare($sql);
	if(!$stmt)
		return false;

	if( !$stmt->execute( Array(':email' => $email) ) )
		return false;

	$row = $stmt->fetch(PDO::FETCH_ASSOC);
	if(!$row)
	{
		$stmt->closeCursor();

		return false;
	}

	if($row['active'] != 1)
		return false;

	$stmt->closeCursor();

	$pass = $row['pass'];

	if( sha1($pass . $data) == $hash )
		return true;

	return false;
}

function auth_query_perms($email)
{
	global $auth_db, $DEFAULT_PERMS;

	if($email == '')
		return $DEFAULT_PERMS;

	$sql = "SELECT admin, view_db, modify_db, admin_db FROM users WHERE email = :email";

	$stmt = $auth_db->prepare($sql);
	if(!$stmt)
		return $DEFAULT_PERMS;

	if( !$stmt->execute( array(':email' => $email) ) )
		return $DEFAULT_PERMS;

	$row = $stmt->fetch(PDO::FETCH_ASSOC);
	if(!$row)
	{
		$stmt->closeCursor();

		return $DEFAULT_PERMS;
	}

	$stmt->closeCursor();

	return $row;
}

function auth_query_user($email, $target_email)
{
	global $auth_db;

	$perms = auth_query_perms($email);

	if($perms['admin'] != true)
		return array();

	$sql = "SELECT email, name, active, type, last_login FROM users WHERE " .
		"email = :email";

	$stmt = $auth_db->prepare($sql);
	if(!$stmt)
		return false;

	if( !$stmt->execute( array('email' => $target_email) ) )
		return false;

	// Check we got back a row
	$user = $stmt->fetch(PDO::FETCH_ASSOC);

	$stmt->closeCursor();

	return $user;
}

function auth_query_users($email)
{
	global $auth_db;

	$perms = auth_query_perms($email);

	if($perms['admin'] != true)
		return array();

	$sql = "SELECT email, name, active, type, last_login FROM users";

	$stmt = $auth_db->prepare($sql);
	if(!$stmt)
		return false;

	if( !$stmt->execute() )
		return false;

	$users = array();
	while( $row = $stmt->fetch(PDO::FETCH_ASSOC) )
		$users[] = $row;

	$stmt->closeCursor();

	return $users;
}

function auth_modify_user($email, $target_email, $new_email,
	$name, $pass, $new_perms)
{
	global $auth_db;

	$perms = auth_query_perms($email);

	if($perms['admin'] != true)
		return array();

	$sql = '';
	if($pass == '')
	{
		$sql = "UPDATE users SET name = :name, email = :new_email, " .
		"admin = :admin, view_db = :view_db, modify_db = :modify_db, " .
		"admin_db = :admin_db WHERE email = :email";
	}
	else
	{
		$sql = "UPDATE users SET name = :name, email = :new_email, " .
		"admin = :admin, view_db = :view_db, modify_db = :modify_db, " .
		"admin_db = :admin_db, pass = :pass WHERE email = :email";
	}

	$vars = array();
	$vars['email'] = $target_email;
	$vars['new_email'] = $new_email;
	$vars['name'] = $name;
	if($pass != '')
		$vars['pass'] = $pass;

	//print_r($new_perms);

	foreach($new_perms as $key => $value)
		$vars[":$key"] = $value == 1 ? true : false;

	//print_r($vars);

	$stmt = $auth_db->prepare($sql);
	if(!$stmt)
		return false;

	if( !$stmt->execute($vars) )
		return false;

	$stmt->closeCursor();

	return true;
}

function auth_validate_request($email, $action)
{
	$perms = auth_query_perms($email);

	if($action['action'] == 'select')
		return $perms['view_db'];
	else if($action['action'] == 'delete')
		return $perms['admin_db'];
	else
		return $perms['modify_db'];

	return false;
}

function auth_make_inactive($email, $target_email)
{
	global $auth_db;

	$perms = auth_query_perms($email);

	if($perms['admin'] != true)
		return false;

	$sql = "UPDATE users SET active = :active WHERE email = :email";

	$stmt = $auth_db->prepare($sql);
	if(!$stmt)
		return false;

	if( !$stmt->execute( array('active' => false, 'email' => $target_email) ) )
		return false;

	$stmt->closeCursor();

	return true;
}

function auth_make_active($email, $target_email)
{
	global $auth_db;

	$perms = auth_query_perms($email);

	if($perms['admin'] != true)
		return false;

	$sql = "UPDATE users SET active = :active WHERE email = :email";

	$stmt = $auth_db->prepare($sql);
	if(!$stmt)
		return false;

	if( !$stmt->execute( array('active' => true, 'email' => $target_email) ) )
		return false;

	$stmt->closeCursor();

	return true;
}

?>
