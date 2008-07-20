/******************************************************************************\
*  website/js/database.js                                                      *
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

var skills_combo = new JComboBox();

function db_interface_refresh()
{
	var label = new JLabel();
	label.setAlignment('right');
	label.setValue("Filter: ");

	var label2 = new JLabel();
	label2.setAlignment('right');
	label2.setValue("Search: ");
	label2.setBackgroundColor('#282828');

	Signal.connect(skills_combo, 'currentTextChanged', label, 'setValue');

	skills_combo.addItem('', '全');

	var search_edit = new JLineEdit();

	var searchLayout = new JGridLayout();
	searchLayout.setBorderWidth(1);
	searchLayout.addWidget(label, 0, 0);
	searchLayout.addWidget(label2, 1, 0);
	searchLayout.addWidget(skills_combo, 0, 1);
	searchLayout.addWidget(search_edit, 1, 1);

	var label3 = new JLabel();
	label3.setValue("Skill List Here");

	var skillListLayout = new JVBoxLayout();
	skillListLayout.addLayout(searchLayout);
	skillListLayout.addWidget(label3);

	//var html  = "<label>Skills:</label>\n";
	/*
	html += "<div class='select' id='skills_dropdown'>\n";
	html += "\t<a onclick='observeOpener(this)' " +
		"class='dd-opener'>- select -</a>\n";
	html += "\t<a onclick='observeOpener(this)' " +
		"class='dd-opener pulldown_arrow'>&nbsp;</a>\n";
	html += "\t<ul id='skill_list' class='selectbox_scrollable' " +
		"style='width:200px'>\n";
	html += "\t</ul>\n";
	html += "<input type='hidden' name='skill' value=''/>\n";
	html += "</div>\n";
	*/
	//html += skills_combo.render();
	//html += "&nbsp;<a class='add'><img src='theme/default/blank.png'/></a>\n";
	//html += "<a class='add'><img src='theme/default/blank.png'/></a>\n";

	$('interface').update( '<center>' + skillListLayout.render() + '</center>' );
	skillListLayout.postRender();

	var action = {
		action: 'select',
		table: 'db_expert',
		columns: ['id', 'name_en', 'name_ja'],
		user_data: 'expert_cache',
		order_by: [{column: 'name_en', direction: 'asc'}]
	}

	/*
	var columns = ['id', ('name_' + lang), 'icon'];
	var action = { action: 'select', table: 'db_skills', columns: columns,
		order_by: [ { column: 'expert', direction: 'asc' },
			{ column: 'class', direction: 'asc' },
			{ column: 'rank', direction: 'asc' } ]
	};
	*/

	var auth = { user_data: { action: 'dropdown_fill', div_id: 'skill_list' } };

	var data = { actions: [ action ], auth: auth };

	new Ajax.Request('backend.php', { method:'post',
		parameters: { request: $H(data).toJSON() },
		onSuccess: function(transport, json) {

			if(transport.responseText != '')
				alert(transport.responseText);

			if(json[0].error)
				alert(json[0].error);

			//var json = transport.responseText.evalJSON()

			for(var i = 0; i < json[0].rows.length; i++)
			{
				var row = json[0].rows[i];

				var name = (lang == 'en') ? row.name_en : row.name_ja;

				skills_combo.addItem('icons/skills/icon16_' +
					row.icon + '.png', name, row.id);
			}

			/*
			var html = '';
			if(lang == 'en')
			{
				for(var i = 0; i < json[0].rows.length; i++)
				{
					html += '<li' + (i == 0 ? ' style="border-width: 0px;"' : '')
						+ '><span class="sv">' + json[0].rows[i].id
						+ '</span><img src="icons/skills/icon16_' + json[0].rows[i].icon
						+ '.png"/><a>' + json[0].rows[i].name_en + '</a></li>';
				}
			}
			else
			{
				for(var i = 0; i < json[0].rows.length; i++)
				{
					html += '<li' + (i == 0 ? ' style="border-width: 0px;"' : '')
						+ '><span class="sv">' + json[0].rows[i].id
						+ '</span><img src="icons/skills/icon16_' + json[0].rows[i].icon
						+ '.png"/><a>' + json[0].rows[i].name_ja + '</a></li>';
				}
			}

			$('skill_list').update(html);

			registerDropDown( $('skill_list') );
			*/
		}
	});
}

function db_interface_currentView()
{
	return this.m_currentView;
}

function db_interface_setCurrentView(view)
{
	// TODO: Check the view is valid
	this.m_currentView = view;;
}

function DatabaseInterface()
{
	this.m_currentView = 'skills';
	this.currentView = db_interface_currentView;
	this.setCurrentView = db_interface_setCurrentView;

	this.refresh = db_interface_refresh;
}
