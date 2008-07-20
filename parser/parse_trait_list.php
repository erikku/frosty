<?php

/******************************************************************************\
*  parser/parse_trait_list.php                                                 *
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
$handle = fopen('239.utf.html', 'r');

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

$expr = "/" . preg_quote("<td class=\"style_td\" style=\"width:120px;\">", '/') .
	"([^\\<]+)" . preg_quote("</td><td class=\"style_td\" style=\"width:500px;" .
	"\">", '/') . "(.+?)" . preg_quote("</td>", '/') . "/";

if( preg_match_all($expr, $file, $matches, PREG_SET_ORDER) )
{
	foreach($matches as $match)
	{
		$name = $match[1];
		$desc = str_replace("<br />", "\n", $match[2]);

		// Strip span
		$desc = preg_replace("/\\<span(.*?)\\>(.+?)\\<\\/span\\>/", "\\2", $desc);

		// Strip bold
		$desc = preg_replace("/\\<b\\>(.+?)\\<\\/b\\>/", "\\1", $desc);

		$sql = "INSERT INTO db_trait (name_ja, desc_ja) VALUES(:name_ja, :desc_ja)";

		$stmt = $db->prepare($sql);
		if(!$stmt)
		{
			print("MySQL STMT Error!\n");
			exit(0);
		}

		$values['name_ja'] = $name;
		$values['desc_ja'] = $desc;
		if( !$stmt->execute($values) )
		{
			print("MySQL Execute Error!\n");
			exit(0);
		}
	}
}

$count = count($matches);
echo "There are $count traits\n";

?>
