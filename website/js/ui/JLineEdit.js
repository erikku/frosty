/******************************************************************************\
*  website/js/ui/JLineEdit.js                                                  *
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

function JLineEdit_render()
{
	return '<input type="text" id="' + this.mObjectName + '" value="' +
		this.mText.escapeHTML() +
		"\" onkeyup=\"JLineEdit_handleTextChanged('" + this.mObjectName +
		"')\" onchange=\"JLineEdit_handleTextChanged('" + this.mObjectName +
		"')\"/>";
}

function JLineEdit_setText(text)
{
	this.mText = text;

	if( $(this.mObjectName) )
		$(this.mObjectName).value = text;
}

function JLineEdit_text()
{
	return this.mText;
}

function JLineEdit_handleTextChanged(obj_name)
{
	var obj = mJObjectCollection.get(obj_name);

	var new_text = Form.Element.getValue(obj_name);
	if(obj.mText == new_text)
		return;

	obj.mText = new_text;
	if(!obj.mBlockSignals)
		obj.currentTextChanged(obj.mText, obj);
}

function JLineEdit_initialize($super, parent)
{
	$super(parent);
	this.mText = '';
}

var JLineEdit = Class.create(JWidget, {
	initialize: JLineEdit_initialize,
	setText: JLineEdit_setText,
	text: JLineEdit_text,
	render: JLineEdit_render,

	/* Signals */
	currentTextChanged: function(text, sender){}
});
