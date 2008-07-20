/******************************************************************************\
*  website/js/ui/JLabel.js                                                     *
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

function JLabel_render()
{
	var bg_color = '';
	if(this.mBackgroundColor != '')
		bg_color = 'background-color:' + this.mBackgroundColor + ';';

	// top right bottom left
	var padding = 'padding: 6px 6px 6px 6px;';

	return '<div id="' + this.mObjectName + '" style="text-align:' +
		(this.mAlignment || 'left') + ';' + bg_color + padding + '">' +
		this.mValue.escapeHTML() + '</div>';
}

function JLabel_setValue(value)
{
	this.mValue = value;

	if( $(this.mObjectName) )
		$(this.mObjectName).update( value.escapeHTML() );
}

function JLabel_setAlignment(value)
{
	this.mAlignment = value;

	if( $(this.mObjectName) )
		$(this.mObjectName).setStyle({textAlign: value});
}

function JLabel_value()
{
	return this.mValue;
}

function JLabel_setBackgroundColor(color)
{
	this.mBackgroundColor = color;

	if( $(this.mObjectName) )
		$(this.mObjectName).setStyle({backgroundColor: color});
}

function JLabel_initialize($super, parent)
{
	$super(parent);
	this.mValue = '';
	this.mBackgroundColor = '';
}

var JLabel = Class.create(JWidget, {
	initialize: JLabel_initialize,
	setValue: JLabel_setValue,
	setAlignment: JLabel_setAlignment,
	setBackgroundColor: JLabel_setBackgroundColor,
	value: JLabel_value,
	render: JLabel_render,
});
