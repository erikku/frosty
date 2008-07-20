/******************************************************************************\
*  website/js/ui/JObject.js                                                    *
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

var mJObjectCollection = new Hash();

function handleSignal(obj, func)
{
	if( !Object.isString(obj) )
		obj = obj.objectName();

	eval("{ var temp_obj = mJObjectCollection.get('" + obj + "');" +
		"temp_obj." + func + "(temp_obj); }");
}

function JObject_setObjectName(object_name)
{
	mJObjectCollection.unset(this.mObjectName);
	this.mObjectName = object_name;
	mJObjectCollection.set(object_name, this);
}

function JObject_objectName()
{
	return this.mObjectName;
}

function JObject_render()
{
	return '';
}

function JObject_postRender()
{
	for(var i = 0, len = this.mChildren.length; i < len; i++)
		this.mChildren[i].postRender();
}

function JObject_addChild(child)
{
	this.mChildren.push(child);
}

function JObject_initialize(parent)
{
	this.mChildren = [];
	this.mParent = parent;
	this.mBlockSignals = false;
	this.mObjectName = 'jobject_' + (mJObjectCollection.keys().size() + 1);
	mJObjectCollection.set(this.mObjectName, this);

	if(parent)
		parent.addChild(this);
}

var JObject = Class.create({
	initialize: JObject_initialize,
	addChild: JObject_addChild,
	setObjectName: JObject_setObjectName,
	render: JObject_render,
	postRender: JObject_postRender,
	objectName: JObject_objectName
});
