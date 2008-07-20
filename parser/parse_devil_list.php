<?php

/******************************************************************************\
*  parser/parse_devil_list.php                                                 *
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

$file = '';
$handle = fopen('207.utf.html', 'r');

while( !feof($handle) )
	$file .= fgets($handle);

fclose($handle);

require_once('config.inc');

$dsn = "mysql:dbname={$DB['db']};host={$DB['host']};charset=UTF-8";

try
{
   	$db = new PDO($dsn, $DB['user'], $DB['pass']);
}
catch(PDOException $e)
{
	$err = "MySQL Error: " . $e->getMessage();
	print("$err\n");

	exit(0);
}

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$db->setAttribute(PDO::ATTR_CASE, PDO::CASE_NATURAL);

function handle_race($db, $race, $again)
{
	$sql = "SELECT id FROM db_race WHERE name_ja = :name_ja";

	$stmt = $db->prepare($sql);
	if(!$stmt)
		return -1;

	$values['name_ja'] = $race;
	if( !$stmt->execute($values) )
		return -1;

	while( $row = $stmt->fetch(PDO::FETCH_ASSOC) )
		return $row['id'];

	$stmt->closeCursor();

	$sql = "INSERT INTO db_race SET name_ja = :name_ja";

	$stmt = $db->prepare($sql);
	if(!$stmt)
		return -1;

	$values['name_ja'] = $race;
	if( !$stmt->execute($values) )
		return -1;

	$stmt->closeCursor();

	if($again == 'no')
		return -1;

	return handle_race($db, $race, 'no');
};

$expr = "/" . preg_quote("<tr><td class=\"style_td\" style=\"text-align:center;"
	. "\"><a target=\"_parent\" href=\"http://oekakigamers.com/upload/img/", '/');

$expr .= "[0-9]+\\.[a-z]{3}" . preg_quote("\" rel=\"nofollow\"><img src=\"http://" .
	"oekakigamers.com/upload/img/", '/') . "[0-9]+\\.[a-z]{3}" . preg_quote("" .
	"\" alt=\"http://oekakigamers.com/upload/img/", '/') . "[0-9]+\\.[a-z]{3}" .
	preg_quote("\" /></a></td><td class=\"style_td\" style=\"" .
	"width:20px;\">", '/');

$expr .= "([0-9]+)" . preg_quote("</td><td class=\"style_td\" "
	. "style=\"width:42px;\">", '/');

$expr .= "([^>]+)" . preg_quote("</td><td class=\"style_td\" style=\"width:" .
	"120px;\"><a target=\"_parent\" href=\"", '/') . "[0-9]+" .
	preg_quote(".html#", '/') . "[a-z0-9]+" . preg_quote("\" title=\"", '/');

$expr .= "[^\\\"]+" . preg_quote("\">", '/') . "([^>]+)" . preg_quote("</a>", '/');

$expr .= "/";

echo "$expr\n";
if( preg_match_all($expr, $file, $matches, PREG_SET_ORDER) )
{
	foreach($matches as $match)
	{
		// Get the race ID or insert one
		$id = handle_race($db, $race, 'yes');

		$level = trim($match[1]);
		$race = trim($match[2]);
		$name = trim($match[3]);
		$sql = "INSERT INTO db_devils (`name_ja`, `lvl`, `race`) " .
			"VALUES(:name_ja, :level, :race)";

		$stmt = $db->prepare($sql);
		if(!$stmt)
		{
			print("MySQL STMT Error!\n");
			exit(0);
		}

		$values['name_ja'] = $name;
		$values['level'] = $level;
		$values['race'] = $id;
		if( !$stmt->execute($values) )
		{
			print("MySQL Execute Error!\n");
			exit(0);
		}
	}
}

$count = count($matches);
echo "There are $count devils\n";

?>
