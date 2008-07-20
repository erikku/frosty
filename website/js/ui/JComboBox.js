/******************************************************************************\
*  website/js/ui/JComboBox.js                                                  *
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

function JComboBox_render()
{
	var layout = '<div id="' + this.mObjectName + '" ' +
			'style="text-align: left;">' +
		'<div class="select" id="' + this.mObjectName + '_dropdown">' +
		'<a onclick="observeOpener(this)" class="dd-opener">' +
		this.currentText() + '</a>' +
		'<a onclick="observeOpener(this)" class="dd-opener pulldown_arrow">' +
			'&nbsp;</a>' +
		'<ul id="' + this.mObjectName + '_values" ' +
		'class="selectbox_scrollable" style="width:200px">';

	for(var i = 0, len = this.mItems.length; i < len; i++)
		layout += this.mItems[i].layout;

	layout += '</ul>' +
		'<input type="hidden" name="' + this.mObjectName +
			'_value" value="' + this.mValue + '" ' + '/>' +
		'</div>' +
		'</div>';

	return layout;
}

function JComboBox_postRender()
{
	registerDropDown( $(this.mObjectName + '_dropdown') );

	for(var i = 0, len = this.mChildren.length; i < len; i++)
		this.mChildren[i].postRender();
}

function JComboBox_setCurrentIndex(index)
{
	this.mValue = index;

	if( $(this.mObjectName + '_dropdown') )
		setDDValue( $(this.mObjectName + '_dropdown'), index,
			this.currentText() );
}

function JComboBox_currentIndex()
{
	return this.mValue;
}

function JComboBox_currentText()
{
	if(this.mValue < 0 || this.mValue >= this.mItems.length)
		return '';

	return this.mItems[this.mValue].text;
}

function JComboBox_addItem(icon, text, user_data)
{
	var item = '';
	if(this.mItems.length == 0)
		item = '<li style="border-width: 0px;"><span class="sv">';
	else
		item = '<li><span class="sv">';

	item += this.mItems.length + '</span>';

	if(icon != '');
		item += '<img src="' + icon + '"/>';

	item += '<a>' + text + '</a></li>';

	this.mItems.push( {text: text, icon: icon, layout: item,
		user_data: user_data} );

	if(this.mValue < 0)
		this.setCurrentIndex(0);

	if( !$(this.mObjectName + '_values') )
		return;

	var layout = '';
	for(var i = 0, len = this.mItems.length; i < len; i++)
		layout += this.mItems[i].layout;

	$(this.mObjectName + '_values').update(layout)
	registerDropDown( $(this.mObjectName + '_dropdown') );
}

function JComboBox_initialize($super, parent)
{
	$super(parent);
	this.mValue = -1;
	this.mItems = [];
}

var JComboBox = Class.create(JWidget, {
	initialize: JComboBox_initialize,
	addItem: JComboBox_addItem,
	render: JComboBox_render,
	postRender: JComboBox_postRender,
	setCurrentIndex: JComboBox_setCurrentIndex,
	currentIndex: JComboBox_currentIndex,
	currentText: JComboBox_currentText,

	/* Signals */
	currentIndexChanged: function(id, sender){},
	currentTextChanged: function(text, sender){}
});
