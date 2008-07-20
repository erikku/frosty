<?php

/******************************************************************************\
*  website/lib/pages/login.inc                                                 *
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
<br/><br/><br/><br/>
<center>
	<form id='login_form' action='<?php echo $_SERVER['PHP_SELF']; ?>' method='post'>
	<table class='login'>
	<thead>
	<tr><td colspan='2'>
			<?php printf($TRANS['login_title'], global_site_name(),
				$TRANS['login']); ?>
	</td></tr>
	</thead>
	<tbody>
	<tr>
		<th><label for='email'><?php print($TRANS['email_address']); ?>:
			</label></th>
		<td><input type='text' id='email' name='email'/></td>
	</tr>
	<tr>
		<th><label for='pass'><?php print($TRANS['password']); ?>:</label></th>
		<td><input type='password' id='pass' name='pass'/></td>
	</tr>
	<tr>
		<td colspan='2'><input type='submit' class='submit'
			value='<?php print($TRANS['login']); ?>'/></td>
	</tr>
	</tbody>
	</table>
	</form>
</center>
<br/><br/><br/><br/>
