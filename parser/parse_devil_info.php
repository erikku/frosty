<?php

/******************************************************************************\
*  parser/parse_devil_info.php                                                 *
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
$handle = fopen('96.utf.html', 'r');

while( !feof($handle) )
	$file .= fgets($handle);

fclose($handle);

//$expr = "/" . preg_quote("<table class=\"style_table", '/') . "[^\\>]*\\>(.+)" .
//	preg_quote("</table>", '/') . "/U";
$expr = "/" . preg_quote("<h3 class=\"title\" id=\"", '/') .
	"[^\\>]*\\>\\s*Lv([0-9]+)\s+(.+)" .
	preg_quote("<a", '/') . "(.*)/";

$race_expr = "/" . preg_quote("<H1 class=\"title\">- ", '/') . "(.+)" .
	preg_quote(" -</H1>", '/') . "/";

preg_match($race_expr, $file, $matches);
$race = trim($matches[1]);

function handle_devil($level, $name, $data)
{
	global $race;

	$lnc_expr = "/" . preg_quote("<th class=\"style_th\">LNC</th><td class=\"" .
	"style_td\">", '/') . "([^\\<]+)" . preg_quote("</td>", '/') . "/";

	$lnc = 'Neutral';
	if( preg_match($lnc_expr, $data, $match) )
		$lnc = $match[1];

	// HP //
	$hp_expr = "/" . preg_quote("<th class=\"style_th\">HP</th><td class=\"" .
		"style_td\">", '/') . "([0-9]+)" . preg_quote("</td>", '/') . "/";

	$hp = 0;
	if( preg_match($hp_expr, $data, $match) )
		$hp = $match[1];

	// MP //
	$mp_expr = "/" . preg_quote("<th class=\"style_th\">MP</th><td class=\"" .
		"style_td\">", '/') . "([0-9]+)" . preg_quote("</td>", '/') . "/";

	$mp = 0;
	if( preg_match($mp_expr, $data, $match) )
		$mp = $match[1];

	// MAG //
	$mag_expr = "/" . preg_quote("<th class=\"style_th\">MAG</th><td class=\"" .
		"style_td\">", '/') . "([0-9]+)" . preg_quote("</td>", '/') . "/";

	$mag = 0;
	if( preg_match($mag_expr, $data, $match) )
		$mag = $match[1];

	// Skill //
	$skill_expr = "/" . preg_quote("<th class=\"style_th\">スキル</th><td class=\"" .
		"style_td\" colspan=\"11\">", '/') . "(.+)" . preg_quote("</td>", '/') . "/";

	$skills = array();
	if( preg_match($skill_expr, $data, $match) )
	{
		$skill = mb_substr($match[1], 0, strpos($match[1], "</td>"));
		$skill = str_replace("<br />", ' / ', $skill);
		$skill = preg_replace("/\\(体得スキル.*個\\)/", '', $skill);
		$temp_skills = explode('/', $skill);
		foreach($temp_skills as $skill)
		{
			if(trim($skill) == '')
				continue;

			$skill_level = 0;
			$skill_name = trim($skill);
			if( preg_match("/^(.*)\\(([0-9]+)\\).*/", $skill, $skill_match) )
			{
				$skill_name = trim($skill_match[1]);
				$skill_level = $skill_match[2];
			}
			$skills[] = "$skill_name [$skill_level]";
		}
	}

	echo "$name ($race) - $lnc ($hp, $mp, $mag)\n";
	echo "Skills: " . implode(', ', $skills) . "\n";
}

echo "$expr\n";
if( preg_match_all($expr, $file, $matches, PREG_SET_ORDER | PREG_OFFSET_CAPTURE) )
{
	foreach($matches as $match)
	{
		$level = trim($match[1][0]);
		$name = trim($match[2][0]);
		$data = mb_substr($file, $match[3][1]);

		handle_devil($level, $name, $data);
	}
}

?>
