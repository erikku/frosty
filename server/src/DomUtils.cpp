/******************************************************************************\
*  server/src/DomUtils.cpp                                                     *
*  Copyright (C) 2006-2008 John Eric Martin <john.eric.martin@gmail.com>       *
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

#include "DomUtils.h"

#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>

QList<QDomNode> elementsByXPath(const QDomDocument& doc, const QString& xpath)
{
	if( xpath.left(1) == "/" )
	{
		QStringList path = xpath.mid(1).split("/");
		if( path.count() )
			return elementsByXPathNode(doc.documentElement(), xpath);
	}
	else
	{
		QString lastNode = xpath.split("/").last();
		if( !lastNode.isEmpty() )
			return QList<QDomNode>();

		QList<QDomNode> nodes;

		QString node_path;
		QDomNodeList list = doc.elementsByTagName(lastNode);
		for(int i = 0; i < list.count(); i++)
		{
			node_path = nodeToXPath( list.at(i) );
			if( node_path.contains(xpath) )
				nodes << list.at(i);
		}

		return nodes;
	}

	return QList<QDomNode>();
};

QDomNode childElementByName(const QDomNode& node, const QString& name)
{
	for(int i = 0; i < node.childNodes().count(); i++)
	{
		if( !node.childNodes().at(i).isElement() )
			continue;
		if( node.childNodes().at(i).toElement().tagName() == name )
			return node.childNodes().at(i);
	}

	return QDomNode();
};

QList<QDomNode> childElementsByName(const QDomNode& node, const QString& name)
{
	QList<QDomNode> nodes;
	for(int i = 0; i < node.childNodes().count(); i++)
	{
		if( !node.childNodes().at(i).isElement() )
			continue;
		if( node.childNodes().at(i).toElement().tagName() == name )
			nodes << node.childNodes().at(i);
	}

	return nodes;
};

QList<QDomNode> elementsByXPathNode(const QDomNode& node, const QString& xpath)
{
	QStringList path;
	if( xpath.left(1) == "/" )
		path = xpath.mid(1).split("/");
	else
		path = xpath.split("/");

	if( !path.count() )
		return QList<QDomNode>();

	QList<QDomNode> nodes;
	for(int i = 0; i < node.childNodes().count(); i++)
	{
		if( !node.childNodes().at(i).isElement() )
			continue;
		if( node.childNodes().at(i).toElement().tagName() == path.first() )
			nodes << node.childNodes().at(i);
	}

	path.removeFirst();
	if( path.count() )
	{
		QList<QDomNode> finalNodes;

		foreach(QDomNode child, nodes)
			finalNodes << elementsByXPathNode( child, path.join("/") );

		return finalNodes;
	}

	return nodes;
};

QString nodeToXPath(const QDomNode& node)
{
	if( !node.isElement() )
		return QString();

	QDomNode current = node;
	QString path;

	while( !current.parentNode().isNull() )
	{
		current = current.parentNode();
		if( current.isElement() )
			path = current.toElement().tagName() + "/" + path;
	}

	path = "/" + path + node.toElement().tagName();

	return path;
};
