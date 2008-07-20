/******************************************************************************\
*  website/js/dropdown.js                                                      *
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

//initialize dropdowns
var hideDropDowns = function()
{
	var dropdowns = document.getElementsByClassName('select');

	for(var i = 0; i < dropdowns.length; i++)
		dropdowns[i].addClassName('hide-dropdown');

	var dropdowns = document.getElementsByClassName('select_active');

	for(var i = 0; i < dropdowns.length; i++)
	{
		dropdowns[i].addClassName('select');
		dropdowns[i].addClassName('hide-dropdown');
		dropdowns[i].removeClassName('select_active');
	}
}

Event.observe(window, 'load', hideDropDowns, false);

//observe dropdown opener
var observeOpener = function(opener)
{
	var dropdown = opener.parentNode;

	if( dropdown.hasClassName('hide-dropdown') )
	{
		hideDropDowns();
		dropdown.removeClassName('select');
		dropdown.addClassName('select_active');
		dropdown.removeClassName('hide-dropdown');
	}
	else
	{
		dropdown.removeClassName('select_active');
		dropdown.addClassName('select');
		dropdown.addClassName('hide-dropdown');
	}
}

//observing dropdown option
var observeOption = function(event)
{
	var option = event.element();
	var value = option.childNodes[0].nodeValue;
	var value_span = option.parentNode.getElementsByClassName('sv')[0];
	var itemid = value_span.childNodes[0].nodeValue;
	var selectbox = event.findElement('ul');
	if(!selectbox)
	{
		event.stop();
		return;
	}

	setDDValue(selectbox.parentNode, itemid, value);

	hideDropDowns();
	event.stop();
}

function registerDropDown(element)
{
	var options = element.getElementsByTagName('a');
	if(!options)
		return;

	for(var i = 0; i < options.length; i++)
		Event.observe(options[i], 'click', (observeOption), false);
}

//observing dropdown options
var observeDropDownOptions = function(event)
{
	var selects = document.getElementsByClassName('selectbox');

	for(var i = 0; i < selects.length; i++)
		registerDropDown(selects[i]);

	var selects = document.getElementsByClassName('selectbox_scrollable');

	for(var i = 0; i < selects.length; i++)
		registerDropDown(selects[i]);

	event.stop();

}

//Event.observe(window, 'load', (observeDropDownOptions), true);

//setting dropdown value into hiddne field
function setDDValue(dropdown, itemid, value)
{
	dropdown.getElementsByTagName('a')[0].update(value);

	var input = dropdown.getElementsByTagName('input')[0];
	if(input)
		input.value = itemid;

	var obj_name = dropdown.parentNode.identify();
	var obj = mJObjectCollection.get(obj_name);

	obj.mValue = itemid;
	if(!obj.mBlockSignals)
	{
		obj.currentIndexChanged(itemid);
		obj.currentTextChanged( obj.currentText() );
	}
}

//closing dropdown by ESC
var onKeyPress = function(event)
{
	switch(event.keyCode)
	{
		case Event.KEY_ESC:
			hideDropDowns(event);
			event.stop();
			break;
	}
}

Event.observe(document, "keypress", (onKeyPress), true);

//closing dropdown by click out of it
var hideDropDownsOnClick = function(event)
{
	if(event.element().tagName.toLowerCase() == 'a')
		return;

	var dropdowns = document.getElementsByClassName('select_active');
	for(var i = 0; i < dropdowns.length; i++)
	{
		dropdowns[i].addClassName('select');
		dropdowns[i].addClassName('hide-dropdown');
		dropdowns[i].removeClassName('select_active');
	}
}

Event.observe(document, "click", (hideDropDownsOnClick), true);
