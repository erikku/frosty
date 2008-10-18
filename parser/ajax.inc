<?php

/******************************************************************************\
*  parser/ajax.inc                                                             *
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

require_once 'Snoopy.class.php';

function get_password($prompt)
{
	$ostty = `stty -g`;
	system(
		"stty -echo -icanon min 1 time 0 2>/dev/null || " .
		"stty -echo cbreak"
	);

	echo "$prompt: ";

	// Get rid of newline when reading stdin
	$pass = substr(fgets(STDIN), 0, -1);

	echo "\n";
	system("stty $ostty");

	return $pass;
}

class ajax
{
	var $url;
	var $email;
	var $pass;
	var $salt;

	function request($action)
	{
		$snoopy = new Snoopy;
		$snoopy->curl_path = '/usr/bin/curl';
		$snoopy->curl_cmdline_params = ' -s --sslv3 --cacert ../client/res/ca.crt';
		$snoopy->_maxlinelen = 1048576;

		$request['actions'] = array($action);

		$post['request'] = json_encode($request);
		if($this->email != '' && $this->pass != '')
		{
			$post['email'] = $this->email;
			$post['pass'] = sha1(sha1($this->salt .
				$this->pass) . $post['request']);
		}

		// Fix the error stuff, errors are returned inside the request number
		if( !($page = $snoopy->submit($this->url, $post)) )
		{
			$error['error'] = "Snoopy Error: " . $snoopy->error;

			return $error;
		}

		foreach($snoopy->headers as $header)
		{
			$header = trim($header);
			if(substr($header, 0, 6) == 'X-JSON')
			{
				return json_decode(substr($header, 9,
					strlen($header) - 10), true);
			}
		}

		$error['error'] = "Error parsing headers";

		return $error;
	}

	function login()
	{
		$backend = 'https://gigadelic.homelinux.net:55517/backend.php';
		$default_email = 'john.eric.martin@gmail.com';

		echo "Please enter the following info (leaving an entry blank will " .
			"use the value shown in brackets)\n\n";

		echo "Backend URL [$backend]: ";
		$this->url = fgets(STDIN);
		$this->url = trim($this->url);

		if($this->url == '')
			$this->url = $backend;

		// Find the salt to use
		{
			// This could be error prone
			$response = $this->request( array('action' => 'salt') );
			$this->salt = $response[0]['salt'];
		}

		echo "Email [$default_email]: ";
		$this->email = fgets(STDIN);
		$this->email = trim($this->email);

		if($this->email == '')
			$this->email = $default_email;

		$this->pass = get_password("Password");
		$this->pass = trim($this->pass);
	}

	function requestID($table, $column, $value)
	{
		$action['action'] = 'select';
		$action['table'] = $table;
		$action['columns'] = array('id');
		$action['where'] = array( array('column' => $column, 'type' => 'equals',
			'value' => $value) );
		$action['where_type'] = 'and';
		$action['limit'] = 1;

		$result = $this->request($action);
		$result = $result[0];

		if( array_key_exists('error', $result) )
			return -1;

		$rows = $result['rows'];
		if( count($rows) != 1 )
			return -1;

		return $rows[0]['id'];
	}

	function updateRelation($table, $id, $name_ja, $name_en)
	{
		$action['action'] = 'update';
		$action['table'] = $table;
		$action['columns'] = array('name_ja', 'name_en');
		$action['where'] = array( array('column' => 'id', 'type' => 'equals',
			'value' => $id) );
		$action['where_type'] = 'and';

		$row['name_ja'] = $name_ja;
		$row['name_en'] = $name_en;
		$action['rows'] = array($row);

		return $this->request($action);
	}

	function insertRelation($table, $name_ja, $name_en)
	{
		$action['action'] = 'insert';
		$action['table'] = $table;
		$action['columns'] = array('name_ja', 'name_en');

		$row['name_ja'] = $name_ja;
		$row['name_en'] = $name_en;
		$action['rows'] = array($row);

		return $this->request($action);
	}

	function handleRelation($table, $name_ja, $name_en)
	{
		$id = $this->requestID($table, 'name_ja', $name_ja);
		if($id == -1)
		{
			$resp = $this->insertRelation($table, $name_ja, $name_en);

			return $resp[0]['ids'][0];
		}
		else
		{
			$this->updateRelation($table, $id, $name_ja, $name_en);

			return $id;
		}
	}
}

?>
