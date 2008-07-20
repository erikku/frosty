<?php

/******************************************************************************\
*  website/lib/megaten.inc                                                     *
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

// Start the session
session_start();

// Include the database settings
require_once 'config.inc';

class Megaten
{
	var $lang;
	var $layout_callbacks;

	function Megaten()
	{
		global $MEGA_GLOBALS;

		$this->lang = isset($_COOKIE['lang']) ? $_COOKIE['lang'] : 'en';
		if( isset($_POST['lang']) )
			$this->lang = $_POST['lang'];
		if( isset($_GET['lang']) )
			$this->lang = $_GET['lang'];

		if( !in_array( $this->lang, explode(',', $MEGA_GLOBALS['AVALIABLE_LANGS']) ) )
			$lang = $MEGA_GLOBALS['DEFAULT_LANG'];

		// Set the cookies
		setcookie('lang', $this->lang, time() + 60 * 60 * 24 * 30);
	}

	function register_layout_callback($section, $callback)
	{
		$this->layout_callbacks[$section] = $callback;
	}

	function layout($section)
	{
		if( array_key_exists($section, $this->layout_callbacks) )
			$this->layout_callbacks[$section]();
	}
};

if( isset($_POST['logout']) || isset($_GET['logout']) )
{
	// unset any cookies
	setcookie('user', '', time() - 3600);
	setcookie('pass', '', time() - 3600);
	setcookie('email', '', time() - 3600);

	unset($_SESSION['user']);
	unset($_SESSION['pass']);
	unset($_SESSION['email']);
	unset($_SESSION['last_login']);
	session_destroy();
}

if( !isset($_SESSION['user']) && ( ( isset($_POST['email'])
	&& isset($_POST['pass']) ) || isset($_COOKIE['email'])
	&& isset($_COOKIE['pass']) ) )
{
	$email = isset($_POST['email']) ? $_POST['email'] : $_COOKIE['email'];
	$pass  = isset($_POST['pass'])  ? $_POST['pass']  : $_COOKIE['pass'];
	$pass  = sha1($pass);

	$db = new MySQLi();
	$db->connect($DB['host'], $DB['user'], $DB['pass'], $DB['db']);
	$stmt = $db->stmt_init();
	$stmt->prepare("SELECT id, name, pass, last_login FROM users WHERE email = ? LIMIT 1");
	//die($db->error);
	$stmt->bind_param("s", $email);
	$stmt->execute();
	$stmt->bind_result($user_id, $name, $real_pass, $last_login);
	$stmt->fetch();

	if($real_pass == $pass)
	{
		$_SESSION['id'] = $user_id;
		$_SESSION['user'] = $name;
		$_SESSION['pass'] = $ass;
		$_SESSION['last_login'] = $last_login;
	}
	else
	{
		die("password bad: $real_pass != $pass");
	}

	$stmt->close();

	if($real_pass == $pass)
	{
		$_SESSION['email'] = $email;

		$stmt = $db->stmt_init();
		$stmt->prepare("UPDATE users SET last_login = UTC_TIMESTAMP() " .
			"WHERE email = ? LIMIT 1") || die($db->error);
		$stmt->bind_param("s", $email);
		$stmt->execute() || die($db->error);
		$stmt->close();

		setcookie('user',  $name,  time() + 60 * 60 * 24 * 30);
		setcookie('pass',  $pass,  time() + 60 * 60 * 24 * 30);
		setcookie('email', $email, time() + 60 * 60 * 24 * 30);
	}

	$db->close();
}

$inst = new Megaten();

require_once "lang/{$inst->lang}/translation.inc";
require_once "theme/default/theme.inc";

// Load the theme
load_theme();

function layout($section)
{
	global $inst;

	$inst->layout($section);
}

function global_site_name()
{
	global $inst, $MEGA_GLOBALS;

	print($MEGA_GLOBALS['SITE_NAME'][$inst->lang]);
}

?>
