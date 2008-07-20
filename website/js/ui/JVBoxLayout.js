/******************************************************************************\
*  website/js/ui/JVBoxLayout.js                                                *
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

function JVBoxLayout_addLayout(layout)
{
	this.addWidget(layout)
}

function JVBoxLayout_addWidget(widget)
{
	this.mChildren.push(widget);
	this.mLayoutObjects.push([widget]);

	if( $(this.mObjectName) )
	{
		var layout = '';
		for(var i = 0, len = this.mLayoutObjects.length; i < len; i++)
		{
			layout += '<tr>';

			for(var j = 0, len2 = this.mLayoutObjects[i].length; j < len2; j++)
				layout += '<td>' + this.mLayoutObjects[i][j].render() + '</td>';

			layout += '</tr>';
		}

		$(this.mObjectName).update(layout);
		widget.postRender();
	}
}

function JVBoxLayout_initialize($super, parent)
{
	$super(parent);
}

var JVBoxLayout = Class.create(JBoxLayout, {
	initialize: JVBoxLayout_initialize,
	addLayout: JVBoxLayout_addLayout,
	addWidget: JVBoxLayout_addWidget,
});
