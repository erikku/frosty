<?php

/******************************************************************************\
*  website/register.php                                                        *
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

require_once('lib/config.inc');

session_start();

if( isset($_SESSION['hash']) && sha1($MEGA_GLOBALS['IMG_SALT'] .
	$_POST['code'])== $_SESSION['hash'] )
{
	register();
}
else if( isset($_SESSION['hash']) )
{
	header("x-registration-error: Invalid Code");
}

function register()
{
	global $DB;

	$dsn = "mysql:dbname={$DB['db']};host={$DB['host']};charset=UTF-8";
	$db = null;

	try
	{
    	$db = new PDO($dsn, $DB['user'], $DB['pass']);
	}
	catch(PDOException $e)
	{
		header("x-registration-error: MySQL Error");

		return;
	}

	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
	$db->setAttribute(PDO::ATTR_CASE, PDO::CASE_NATURAL);

	$stmt = $db->prepare("SELECT id FROM users WHERE email = :email");
	if(!$stmt)
	{
		header("x-registration-error: MySQL Error");

		unset($db);
		return;
	}

	if( !$stmt->execute( array(':email' => $_POST['email']) ) )
	{
		header("x-registration-error: MySQL Error");

		unset($db);
		return;
	}

	if( ($row = $stmt->fetch(PDO::FETCH_ASSOC)) )
	{
		header("x-registration-error: Email address already in use");

		unset($db);
		return;
	}

	$stmt = $db->prepare("SELECT id FROM users WHERE name = :name");
	if(!$stmt)
	{
		header("x-registration-error: MySQL Error");

		unset($db);
		return;
	}

	if( !$stmt->execute( array(':name' => $_POST['name']) ) )
	{
		header("x-registration-error: MySQL Error");

		unset($db);
		return;
	}

	if( ($row = $stmt->fetch(PDO::FETCH_ASSOC)) )
	{
		header("x-registration-error: User name already in use");

		unset($db);
		return;
	}

	// Insert the account
	$stmt = $db->prepare("INSERT INTO users(email, name, pass) VALUES(" .
		":email, :name, :pass)");
	if(!$stmt)
	{
		header("x-registration-error: MySQL Error");

		unset($db);
		return;
	}

	$info[':email'] = $_POST['email'];
	$info[':name'] = $_POST['name'];
	$info[':pass'] = $_POST['pass'];

	if( !$stmt->execute($info) )
	{
		header("x-registration-error: MySQL Error");

		unset($db);
		return;
	}

	header('content-type: text/html');
	echo "OK";

	exit(0);
}

$letters = "あ,い,う,え,お,か,き,く,け,こ,さ,し,す,せ,そ,た,ち,つ,て,と,な,に,ぬ,ね," .
	"の,は,ひ,ふ,へ,ほ,ま,み,む,め,も,や,ゆ,よ,ら,り,る,れ,ろ,わ,を,ん,が,ぎ,ぐ,げ,ご," .
	"ざ,じ,ず,ぜ,ぞ,だ,で,ど,ば,び,ぶ,べ,ぼ,ぱ,ぴ,ぷ,ぺ,ぽ,ア,イ,ウ,エ,オ,カ,キ,ク,ケ," .
	"コ,サ,シ,ス,セ,ソ,タ,チ,ツ,テ,ト,ナ,ニ,ヌ,ネ,ノ,ハ,ヒ,フ,ヘ,ホ,マ,ミ,ム,メ,モ,ヤ," .
	"ユ,ヨ,ラ,リ,ル,レ,ロ,ワ,ヲ,ン,ガ,ギ,グ,ゲ,ゴ,ザ,ジ,ズ,ゼ,ゾ,ダ,デ,ド,バ,ビ,ブ,ベ," .
	"ボ,パ,ピ,プ,ペ,ポ";

$letters = explode(',', $letters);
$count = count($letters);

$text = '';
for($i = 0; $i < 8; $i++)
	$text .= $letters[rand(0, $count - 1)];

$_SESSION['hash'] = sha1($MEGA_GLOBALS['IMG_SALT'] . $text);

$font = realpath('.') . "/theme/default/font.ttf";

$box = imagettfbbox(20, 0, $font, $text);
$width = $box[2] - $box[6];
$height = $box[3] - $box[7];

$im = imagecreatetruecolor($width, $height);

//$background = imagecolorallocatealpha($im, 255, 255, 255, 0);
$background = imagecolorallocatealpha($im, 255, 255, 255, 127);
$font_color = imagecolorallocate($im, 0, 192, 0);

imagefilledrectangle($im, 0, 0, $width - 1, $height - 1, $background);

// Add the text
imagettftext($im, 20, 0, -$box[6], -$box[7], $font_color, $font, $text);

header("expires: Mon, 23 Jul 1993 05:00:00 GMT");
header("last-modified: " . gmdate("D, d M Y H:i:s") . " GMT");

header("cache-control: no-store, no-cache, must-revalidate");
header("cache-control: post-check=0, pre-check=0", false);
header("x-registration-salt: {$MEGA_GLOBALS['PASS_SALT']}");

header("pragma: no-cache");

header('content-type: image/png');

imagepng($im);
imagedestroy($im);

?>
