<?php

/******************************************************************************\
*  website/lib/pages/database.inc                                              *
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
<div id='interface'>
</div>
<script type='text/javascript'>
	// Set the database language
	var lang = '<?php echo $inst->lang; ?>';

	// Create the database interface
	var db = new DatabaseInterface();

	// Refresh the interface contents
	db.refresh();
</script>
<script type="text/javascript">
<!--
/*
var devil_rows = [
	{ name_en: 'Odin', name_ja: 'オーディン', level: 70, affinity_name_ja: '刀',
		affinity_name_en: 'Swords' },
	{ name_en: 'Titania', name_ja: 'ティターニア', level: 57, affinity_name_ja: '魔法',
		affinity_name_en: 'Magic' } ];

var relations = { db_affinity: { id: 'affinity', foreign_id: 'id',
	columns: { name_en: 'affinity_name_en', name_ja: 'affinity_name_ja' } } };
var where_id = { type: 'greater_then', column: 'id', value: 0 };
var select_action = { action: 'select', table: 'db_skills', offset: 0,
	columns: ['id', 'name_en', 'name_ja', 'icon'], where: [ where_id ], limit: 2,
	group_by: [ { column: 'affinity_name_en', direction: 'asc' } ],
	order_by: [ { column: 'name_en', direction: 'asc' },
		{ column: 'name_ja', direction: 'desc' } ],
	relations: relations, where_type: 'and' };

var insert_action = { action: 'insert', table: 'db_devils', rows: devil_rows,
	relations: relations };

var actions = [ select_action ];
var auth = { email: 'cpuwhiz@gmail.com',
	password: 'ecb294def07fa5adcab95e121ac0d56ed32fcdb5',
	user_data: { action: 'table_fill', div_id: 'joson_test' } };

var data = { actions: actions, auth: auth };
*/
-->
</script>
<br/><br/><br/><br/>
<div id='jwidget_test'></div>
<br/><br/><br/><br/>
