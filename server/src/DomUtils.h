/******************************************************************************\
*  server/src/DomUtils.h                                                       *
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

#ifndef __DomUtils_h__
#define __DomUtils_h__

#include <QtCore/QList>
#include <QtCore/QStringList>

#include <QtXml/QDomNode>
#include <QtXml/QDomDocument>

QDomNode childElementByName(const QDomNode& node, const QString& name);
QList<QDomNode> childElementsByName(const QDomNode& node, const QString& name);

QString nodeToXPath(const QDomNode& node);
QList<QDomNode> elementsByXPath(const QDomDocument& doc, const QString& xpath);
QList<QDomNode> elementsByXPathNode(const QDomNode& node, const QString& xpath);

#endif // __DomUtils_h__
