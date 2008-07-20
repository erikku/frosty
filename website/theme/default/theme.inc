<?php

/******************************************************************************\
*  website/theme/default/theme.inc                                             *
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

function default_layout_stylesheet()
{
	print("<link rel='shortcut icon' href='theme/default/favicon.ico'/>\n");
	print("\t<link rel='stylesheet' type='text/css' " .
		"href='theme/default/style.css'/>");
	print("\t<link rel='stylesheet' type='text/css' " .
		"href='theme/default/dropdown.css'/>");
}

function load_theme()
{
	global $inst;

	$inst->register_layout_callback("LAYOUT_STYLESHEET",
		default_layout_stylesheet);
}

?>
