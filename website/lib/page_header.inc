<?php

/******************************************************************************\
*  website/lib/page_header.inc                                                 *
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

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns='http://www.w3.org/1999/xhtml'
	xml:lang='<?php echo $inst->lang; ?>' lang='<?php echo $inst->lang; ?>'>
<head>
	<meta http-equiv="content-type" content="text/html; charset=UTF-8"/>
	<script src='js/prototype.js' type='text/javascript'></script>
	<script src='js/signal.js' type='text/javascript'></script>
	<script src='js/dropdown.js' type='text/javascript'></script>
	<script src='js/ui/JObject.js' type='text/javascript'></script>
	<script src='js/ui/JLayout.js' type='text/javascript'></script>
	<script src='js/ui/JBoxLayout.js' type='text/javascript'></script>
	<script src='js/ui/JGridLayout.js' type='text/javascript'></script>
	<script src='js/ui/JHBoxLayout.js' type='text/javascript'></script>
	<script src='js/ui/JVBoxLayout.js' type='text/javascript'></script>
	<script src='js/ui/JWidget.js' type='text/javascript'></script>
	<script src='js/ui/JLineEdit.js' type='text/javascript'></script>
	<script src='js/ui/JComboBox.js' type='text/javascript'></script>
	<script src='js/ui/JLabel.js' type='text/javascript'></script>
	<script src='js/database.js' type='text/javascript'></script>
	<meta http-equiv='content-script-type' content='text/javascript'/>
	<?php layout("LAYOUT_STYLESHEET"); ?>

	<title><?php global_site_name(); ?> 「Database」</title>
	<script type="text/javascript">
	<!--
		function setLanguage(lang)
		{
			document.getElementById('lang').value = lang;
			document.getElementById('lang_form').submit();
		}
	-->
	</script>
</head>
<body>
<div id='left' class='bar'>
<a href='bank.php'><?php print($TRANS['bank']); ?></a> |
<span><?php print($TRANS['database']); ?></span> |
<a href='recipes.php'><?php print($TRANS['recipes']); ?></a>
</div>
<div id='right' class='bar'>
	<span><?php if( isset($_SESSION['user']) ) print($_SESSION['user']);
		else print("Guest"); ?></span> |
	<a href='account.php'><?php print($TRANS['my_account']); ?></a> |
	<?php

	if( isset($_SESSION['user']) )
	{
		printf("<a class='jlink' " .
			"onclick=\"document.getElementById('logout_form').submit()\">" .
			"{$TRANS['logout']}</a> |\n");
	}
	else
	{
		print("<a class='jlink' " .
			"onclick=\"document.getElementById('login_trigger_form').submit()\">" .
			"{$TRANS['login']}</a> |\n");
	}

	foreach(explode(',', $MEGA_GLOBALS['AVALIABLE_LANGS']) as $value)
		$a[] = "<a class='jlink' onclick=\"setLanguage('$value')\">" .
			"<img alt='$value flag' src='lang/{$value}/icon.png'/></a>";

	print("\t" . implode("&nbsp;\n\t", $a) . "\n");

	?>
</div>
<div id='main' class='main'>
<hr/>
