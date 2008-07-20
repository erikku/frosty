<?php

/******************************************************************************\
*  website/lib/page_footer.inc                                                 *
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
<form id='logout_form' action='<?php echo $_SERVER['PHP_SELF']; ?>' method='post'>
	<input type='hidden' id='logout' name='logout' value='1'/>
</form>
<form id='lang_form' action='<?php echo $_SERVER['PHP_SELF']; ?>' method='post'>
	<input type='hidden' id='lang' name='lang' value='en'/>
</form>
<form id='login_trigger_form' action='<?php echo $_SERVER['PHP_SELF']; ?>' method='post'>
	<input type='hidden' id='login_trigger' name='login' value='1'/>
</form>
<center>
<table width='90%'><tr><td>
<p>
	<a href="http://validator.w3.org/check?uri=referer"><img
		src="http://www.w3.org/Icons/valid-xhtml10-blue"
		alt="Valid XHTML 1.0 Transitional" height="31" width="88"/></a>
	<a href="http://jigsaw.w3.org/css-validator/"><img
		src="http://www.w3.org/Icons/valid-css2-blue"
		alt="Valid CSS Level 2.1!" height="31" width="88"/></a>
</p>
<?php
	if( isset($_SESSION['last_login']) )
		print("\tLast Login: {$_SESSION['last_login']} UTC<br/>");
?>
	Copyright &copy; 2008 CpuWhiz
		&lt;<a href='mailto:cpuwhiz@gmail.com'>cpuwhiz@gmail.com</a>&gt;
</td></tr></table>
</center>
</div>
</body>
</html>
