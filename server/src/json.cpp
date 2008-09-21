/******************************************************************************\
*  server/src/json.cpp                                                         *
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

#include "json.h"

#include <QtCore/QRegExp>
#include <QtCore/QStringList>

#ifdef QT_GUI_LIB
#include <QtGui/QTreeWidgetItem>
#endif

#include <iostream>
using namespace std;

json::json(const QString& string)
{
	pos = 0;
	str = string;
};

QVariant json::parse(const QString& str)
{
	json j(str);
	try {
		return j.parse_any();
	}
	catch(char *e) {
		cerr << e << endl;
	}

	return QVariant();
};

QString json::toJSON(const QVariant& obj)
{
	if(obj.type() == QVariant::Map)
		return mapToJSON(obj);
	else if(obj.type() == QVariant::List)
		return listToJSON(obj);
	else if(obj.type() == QVariant::String)
		return stringToJSON(obj);
	else if( obj.type() == QVariant::Bool )
		return obj.toString(); // obj.toBool() ? "1" : "0";
	else if( obj.canConvert(QVariant::Double) )
		return obj.toString();
	else
		return "null";

	return QString();
};

#ifdef QT_GUI_LIB
QList<QTreeWidgetItem*> json::toTree(const QVariant& obj)
{
	QList<QTreeWidgetItem*> list;

	if(obj.type() == QVariant::Map)
		list << mapToTree(obj);
	else if(obj.type() == QVariant::List)
		list << listToTree(obj);
	else if(obj.type() == QVariant::String)
		list << new QTreeWidgetItem(QStringList() << obj.toString());
	else if(obj.type() == QVariant::Bool)
		list << new QTreeWidgetItem(QStringList() << obj.toString());
	else if( obj.canConvert(QVariant::Double) )
		list << new QTreeWidgetItem(QStringList() << obj.toString());
	else
		list << new QTreeWidgetItem(QStringList() << "null");

	return list;
};

QList<QTreeWidgetItem*> json::mapToTree(const QVariant& obj)
{
	QList<QTreeWidgetItem*> list;

	QVariantMap map = obj.toMap();
	QMapIterator<QString, QVariant> i(map);

	while( i.hasNext() )
	{
		i.next();

		QTreeWidgetItem *item = new QTreeWidgetItem( QStringList() << i.key() );

		if(i.value().type() == QVariant::Map)
			item->addChildren( mapToTree(i.value()) );
		else if(i.value().type() == QVariant::List)
			item->addChildren( listToTree(i.value()) );
		else if(i.value().type() == QVariant::String)
			item->setData(0, Qt::UserRole, i.value());
		else if(i.value().type() == QVariant::Bool)
			item->setData(0, Qt::UserRole, i.value());
		else if( i.value().canConvert(QVariant::Double) )
			item->setData(0, Qt::UserRole, i.value());
		else
			item->setData(0, Qt::UserRole, QString("null"));

		list << item;
	}

	return list;
};
#endif

QString json::mapToJSON(const QVariant& obj)
{
	QStringList result;
	QVariantMap map = obj.toMap();
	QMapIterator<QString, QVariant> i(map);

	while( i.hasNext() )
	{
		i.next();

		QString value = toJSON( i.value() );
		QString key = QString("\"%1\":").arg( i.key() );

		result << (key + value);
	}

	return QString("{%1}").arg( result.join(",") );
};

#ifdef QT_GUI_LIB
QList<QTreeWidgetItem*> json::listToTree(const QVariant& obj)
{
	QList<QTreeWidgetItem*> list;

	QVariantList obj_list = obj.toList();

	for(int i = 0; i < obj_list.count(); i++)
	{
		QVariant value = obj_list.at(i);
		QTreeWidgetItem *item = new QTreeWidgetItem( QStringList() <<
			QString("[%1]").arg(i) );

		if(value.type() == QVariant::Map)
			item->addChildren( mapToTree(value) );
		else if(value.type() == QVariant::List)
			item->addChildren( listToTree(value) );
		else if(value.type() == QVariant::String)
			item->setData(0, Qt::UserRole, value);
		else if(value.type() == QVariant::Bool)
			item->setData(0, Qt::UserRole, value);
		else if( value.canConvert(QVariant::Double) )
			item->setData(0, Qt::UserRole, value);
		else
			item->setData(0, Qt::UserRole, QString("null"));

		list << item;
	}

	return list;
};
#endif

QString json::listToJSON(const QVariant& obj)
{
	QStringList result;
	QVariantList list = obj.toList();

	for(int i = 0; i < list.size(); i++)
		result << toJSON( list.at(i) );

	return QString("[%1]").arg( result.join(",") );
};

QString json::stringToJSON(const QVariant& obj)
{
	// Simple hack for now
	QString result;
	QString src = obj.toString();

	src = src.replace("\"", "\\\"");
	src = src.replace("\\", "\\\\");
	src = src.replace("/", "\\/");
	src = src.replace("\b", "\\b");
	src = src.replace("\f", "\\f");
	src = src.replace("\n", "\\n");
	src = src.replace("\r", "\\r");
	src = src.replace("\t", "\\t");

	for(int i = 0; i < src.length(); i++)
	{
		if( !src[i].toAscii() )
			result += QString("\\u%1").arg( QString::number(src[i].unicode(), 16) );
		else
			result += src[i];
	}

	return QString("\"%1\"").arg(result);
};

QVariant json::parse_any()
{
	int len = str.length();

	QRegExp number_matcher("[-0-9]");

	for(; pos < len; pos++)
	{
		if( str[pos] == QChar(' ') )
			continue;
		else if( str[pos] == QChar('{') ) // Object
			return parse_object();
		else if( str[pos] == QChar('[') ) // Array
			return parse_array();
		else if( str[pos] == QChar('"') ) // String
			return parse_string();
		else if( str[pos] == QChar('n') ) // Null
			return parse_null();
		else if( str[pos] == QChar('t') ) // True
			return parse_true();
		else if( str[pos] == QChar('f') ) // False
			return parse_false();
		else if( number_matcher.exactMatch( str.mid(pos, 1) ) ) // Number
			return parse_number();
		else
			throw QString("Any Parse Error: %1").arg(str.mid(pos)).toLocal8Bit().data();
	}

	return QVariant();
};

QVariant json::parse_object()
{
	int len = str.length();

	if( len <= 0 || str[pos] != QChar('{') )
		throw "Object Call Error";

	QString key;
	QMap<QString, QVariant> map;

	while( str[pos] != QChar('}') )
	{
		pos++;

		for(; pos < len; pos++)
		{
			if( str[pos] == QChar(' ') )
				continue;
			else if( str[pos] == QChar('"') )
			{
				key = parse_string().toString();
				break;
			}
			else
				throw "Object Parse Error";
		}

		// Skip over the :
		for(; pos < len; pos++)
		{
			if( str[pos] == QChar(' ') )
				continue;
			else if( str[pos] == QChar(':') ) // Found the :
				break;
			else
				throw "Object Parse Error";
		}

		pos++;
		map[key] = parse_any();

		// Find the end or another key/value pair
		for(; pos < len; pos++)
		{
			if( str[pos] == QChar(' ') )
				continue;
			else if( str[pos] == QChar(',') ) // Do another key/value pair
				break;
			else if( str[pos] == QChar('}') ) // We are done reading the object
				break;
			else
				throw "Object Parse Error";
		}
	}

	pos++;

	return QVariant(map);
};

QVariant json::parse_array()
{
	int len = str.length();

	if( len <= 0 || str[pos] != QChar('[') )
		throw "Array Call Error";

	QList<QVariant> list;

	while( str[pos] != QChar(']') && pos < len )
	{
		pos++;

		// Skip any whitespace
		while( str[pos] == QChar(' ') )
			pos++;

		// Found the end of the array (this only really happens for an empty array)
		if( str[pos] == QChar(']') )
			break;

		// Found a new element
		if( str[pos] == QChar(',') )
			continue;

		// Add the array element
		list << parse_any();
	}

	pos++;

	return QVariant(list);
};

QVariant json::parse_string()
{
	int len = str.length();

	if( len <= 0 || str[pos] != QChar('"') )
		throw "String Call Error";

	pos++;
	int start = pos;
	int end = pos;
	int index = -1;

	while( (index = str.indexOf(QChar('"'), end)) != -1 )
	{
		end = index;
		if( str[end - 1] != QChar('\\') )
			break;
	}

	// Empty String
	if( end == pos )
	{
		pos = end + 1;

		return QVariant("");
	}

	QString result = str.mid(start, end - start);
	QRegExp unicodeMatcher("(.*)\\\\u([0-9a-f]{4})(.*)");

	while( unicodeMatcher.exactMatch(result) )
		result = unicodeMatcher.cap(1) + QChar( unicodeMatcher.cap(2).toUInt(0, 16) ) +
			unicodeMatcher.cap(3);

	result = result.replace("\\\"", "\"");
	result = result.replace("\\\\", "\\");
	result = result.replace("\\/", "/");
	result = result.replace("\\b", "\b");
	result = result.replace("\\f", "\f");
	result = result.replace("\\n", "\n");
	result = result.replace("\\r", "\r");
	result = result.replace("\\t", "\t");

	pos = end + 1;
	return QVariant(result);
};

QVariant json::parse_null()
{
	int len = str.length();

	if( len <= 0 || str[pos] != QChar('n') )
		throw "Null Call Error";

	QString null_str("null");
	if( (len - pos) < null_str.length() )
		throw "Null Call Error";

	if(str.mid(pos, null_str.length()) != null_str)
		throw "Null Call Error";

	pos += null_str.length();

	return QVariant();
};

QVariant json::parse_true()
{
	int len = str.length();

	if( len <= 0 || str[pos] != QChar('t') )
		throw "True Call Error";

	QString true_str("true");
	if( (len - pos) < true_str.length() )
		throw "True Call Error";

	if(str.mid(pos, true_str.length()) != true_str)
		throw "True Call Error";

	pos += true_str.length();

	return QVariant(true);
};

QVariant json::parse_false()
{
	int len = str.length();

	if( len <= 0 || str[pos] != QChar('f') )
		throw "False Call Error";

	QString false_str("false");
	if( (len - pos) < false_str.length() )
		throw "False Call Error";

	if(str.mid(pos, false_str.length()) != false_str)
		throw "False Call Error";

	pos += false_str.length();

	return QVariant(false);
};

QVariant json::parse_number()
{
	QString num;
	QRegExp num_matcher("[0-9\\s]");
	QRegExp space_matcher("\\s");

	// Negative sign
	if( str[pos] == QChar('-') )
	{
		num = "-";
		pos++;
	}

	while( num_matcher.exactMatch( str.mid(pos, 1) ) )
	{
		if( !space_matcher.exactMatch( str.mid(pos, 1) ) )
			num += str[pos];

		pos++;
	}

	if( str.mid(pos, 1).toLower() == "." )
	{
		num += ".";
		pos++;

		while( num_matcher.exactMatch( str.mid(pos, 1) ) )
		{
			if( !space_matcher.exactMatch( str.mid(pos, 1) ) )
				num += str[pos];

			pos++;
		}
	}
	if( str.mid(pos, 1).toLower() == "e" )
	{
		num += "e";
		pos++;

		if( str[pos] == QChar('+') || str[pos] == QChar('-') )
		{
			num += str[pos];
			pos++;
		}

		while( num_matcher.exactMatch( str.mid(pos, 1) ) )
		{
			if( !space_matcher.exactMatch( str.mid(pos, 1) ) )
				num += str[pos];

			pos++;
		}
	}

	if( num.contains('.') || num.contains('e') )
	{
		bool ok;
		float final = num.toFloat(&ok);
		if(!ok)
			throw "Number Parse Error";

		return final;
	}
	else
	{
		bool ok;
		int final = num.toInt(&ok);
		if(!ok)
			throw "Number Parse Error";

		return final;
	}
};
