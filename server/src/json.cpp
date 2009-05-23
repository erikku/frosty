/******************************************************************************\
*  client/src/json.cpp                                                         *
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
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtGui/QTreeWidgetItem>
#endif

#include <iostream>
using namespace std;

JSONParseError::JSONParseError(const QString& t, const QString& msg,
	const QString& sub, int pos, int len)
{
	mType = t;
	mMessage = msg;
	mSubject = sub;

	mFrom = -1;
	mTo = -1;

	mFromLine = -1;
	mToLine = -1;

	// Calculate the line(s) the error is on
	int max = sub.length();
	if(len <= 0)
		len = max;

	mPos = pos;
	mLen = len;

	if(max <= 0 || pos < 0 || pos > max || (pos + len) > max)
		throw "Invalid values in JSONParseError";

	int stop_pos = pos + len - 1;

	int line = 0;
	int line_start = 0;
	int line_end = 0;

	do
	{
		line++;

		line_end = sub.indexOf(QChar('\n'), line_start);
		if(line_end < 0)
			line_end = max - 1;

		if(pos >= line_start && pos <= line_end)
		{
			mFrom = pos - line_start + 1;
			mFromLine = line;
		}

		if(stop_pos >= line_start && stop_pos <= line_end)
		{
			mTo = stop_pos - line_start + 1;
			mToLine = line;
		}

		line_start = line_end + 1;
	} while(line_start < max && line_end < max);

	if(mFrom < 0 || mFromLine < 1 || mTo < 0 || mToLine < 1)
		throw "Error calculating line numbers in JSONParseError";
}

QString JSONParseError::type() const
{
	return mType;
}

QString JSONParseError::message() const
{
	return mMessage;
}

QString JSONParseError::subject() const
{
	return mSubject;
}

QString JSONParseError::toHTML() const
{
	QString str;
	if( mType.isEmpty() )
		str = QString("JSON parse error at %1-%2: %3").arg(
			mFromLine).arg(mFrom).arg(mMessage);
	else
		str = QString("JSON parse error at %1-%2: %3: %4").arg(
			mFromLine).arg(mFrom).arg(mType).arg(mMessage);

	QString pre = mSubject.left(mPos);
	int line_start = pre.lastIndexOf(QChar('\n')) + 1;

	QString line = mSubject.mid(line_start);

	int len = line.indexOf(QChar('\n'));
	if(len > 80)
		len = 80;

	line = line.left(len);
	line = QString("%1<b>%2</b>%3").arg( line.mid(0, mFrom - 1) ).arg(
		line[mFrom - 1]).arg( line.mid(mFrom) );

	str = QString("%1\n<br/>%2").arg(str).arg(line);

	return str;
}

QString JSONParseError::toString() const
{
	QString str;
	if( mType.isEmpty() )
		str = QString("JSON parse error at %1-%2: %3").arg(
			mFromLine).arg(mFrom).arg(mMessage);
	else
		str = QString("JSON parse error at %1-%2: %3: %4").arg(
			mFromLine).arg(mFrom).arg(mType).arg(mMessage);

	QString pre = mSubject.left(mPos);
	int line_start = pre.lastIndexOf(QChar('\n')) + 1;

	QString line = mSubject.mid(line_start);

	int len = line.indexOf(QChar('\n'));
	if(len > 80)
		len = 80;

	line = line.left(len);

	QString pointer;
	pointer.fill(QChar(' '), mFrom - 1);
	pointer += QChar('^');

	str += QString("\n%1\n%2").arg(line).arg(pointer);

	return str;
}

int JSONParseError::from() const
{
	return mFrom;
}

int JSONParseError::fromLine() const
{
	return mFromLine;
}

int JSONParseError::to() const
{
	return mTo;
}

int JSONParseError::toLine() const
{
	return mToLine;
}

json::json(const QString& string)
{
	pos = 0;
	str = string;
}

QVariant json::parse(const QString& str, bool die)
{
	json j(str);

	try
	{
		QVariant ret = j.parse_any();
		if(j.str.length() > j.pos)
			throw JSONParseError("Parse Error", QString("expected end of the "
				"stream but got '%1' instead").arg(str[j.pos]), str, j.pos);

		return ret;
	}
	catch(JSONParseError e)
	{
		if(!die)
			return QVariant();

#ifdef QT_GUI_LIB
		QMessageBox box(QMessageBox::Critical, "JSON Parse Error", e.toHTML(),
			QMessageBox::Ok);

		box.setTextFormat(Qt::RichText);
		box.exec();

		qApp->quit();
#else
		cout << e.toString().toLocal8Bit().data() << endl;
#endif
	}

	return QVariant();
}

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
}

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
}

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
}
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
		QString key = QString("%1:").arg( stringToJSON(i.key()) );

		result << (key + value);
	}

	return QString("{%1}").arg( result.join(",") );
}

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
}
#endif

QString json::listToJSON(const QVariant& obj)
{
	QStringList result;
	QVariantList list = obj.toList();

	for(int i = 0; i < list.size(); i++)
		result << toJSON( list.at(i) );

	return QString("[%1]").arg( result.join(",") );
}

QString json::stringToJSON(const QVariant& obj)
{
	// Simple hack for now
	QString result;
	QString src = obj.toString();

	// This needs to be first to prevent it from escaping the slash in
	// escaped characters
	src = src.replace("\\", "\\\\");

	src = src.replace("\"", "\\\"");
	src = src.replace("/", "\\/");
	src = src.replace("\b", "\\b");
	src = src.replace("\f", "\\f");
	src = src.replace("\n", "\\n");
	src = src.replace("\r", "\\r");
	src = src.replace("\t", "\\t");

	QString badX = QString::fromUtf8("×");
	for(int i = 0; i < src.length(); i++)
	{
		if( !src[i].toAscii() || badX[0] == src[i] )
			result += QString("\\u%1").arg(src[i].unicode(),
				4, 16, QLatin1Char('0'));
		else
			result += src[i];
	}

	return QString("\"%1\"").arg(result);
}

bool json::isWhitespace(const QChar& c)
{
	if(QChar(' ') == c)
		return true;
	else if(QChar(0x3000) == c)
		return true;
	else if(QChar('\t') == c)
		return true;
	else if(QChar('\n') == c)
		return true;
	else if(QChar('\r') == c)
		return true;

	return false;
}

QVariant json::parse_any()
{
	int len = str.length();

	QRegExp number_matcher("[-0-9]");

	for(; pos < len; pos++)
	{
		if( isWhitespace(str[pos]) )
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
			throw JSONParseError("Any Parse Error", QString("Unexpected "
				"character '%1'").arg(str[pos]), str, pos);
	}

	throw JSONParseError("Any Parse Error", "Unexpected end of stream",
		str + " ", len);

	return QVariant();
}

QVariant json::parse_object()
{
	int len = str.length();

	if(len <= 0 || pos >= len)
		throw JSONParseError("Object Parse Error", "expected '{' but got the "
			"end of the stream instead", str + " ", len);

	if(QChar('{') != str[pos])
		throw JSONParseError("Object Parse Error", QString("expected '{' but "
			"got '%1' instead").arg(str[pos]), str, pos);

	QString key;
	QMap<QString, QVariant> map;
	bool haveKey = false, haveValue = false;
	bool haveSemicolon = false;

	while(true)
	{
		pos++;

		// Skip any whitespace
		while( pos < len && isWhitespace(str[pos]) )
			pos++;

		if(pos >= len)
		{
			if(haveKey && haveValue)
				throw JSONParseError("Object Parse Error", "expected ',' or '}'"
					" but got the end of the stream instead", str + " ", len);
			else if(haveKey && !haveSemicolon)
				throw JSONParseError("Object Parse Error", "expected ':' "
					"but got the end of the stream instead", str + " ", len);
			else if(haveKey && haveSemicolon)
				throw JSONParseError("Object Parse Error", "expected object "
					"value but got the end of the stream "
					"instead", str + " ", len);
			else if( !map.count() )
				throw JSONParseError("Object Parse Error", "expected object "
					"key or '}' but got the end of the stream "
					"instead", str + " ", len);
		}

		if(QChar('}') == str[pos]) // Found the end of the object
		{
			if(map.count() || haveKey)
			{
				if(!haveKey)
					throw JSONParseError("Object Parse Error", "expected "
						"object key but got '}' instead", str, pos);
				else if(haveKey && !haveSemicolon)
					throw JSONParseError("Object Parse Error", "expected "
						"':' but got '}' instead", str, pos);
				else if(haveKey && haveSemicolon && !haveValue)
					throw JSONParseError("Object Parse Error", "expected "
						"object value but got '}' instead", str, pos);
			}

			break;
		}
		else if(QChar(',') == str[pos]) // Found a new element
		{
			if(!haveKey)
				throw JSONParseError("Object Parse Error", "expected "
					"object key but got ',' instead", str, pos);
			else if(haveKey && !haveSemicolon)
				throw JSONParseError("Object Parse Error", "expected "
					"':' but got ',' instead", str, pos);
			else if(haveKey && haveSemicolon && !haveValue)
				throw JSONParseError("Object Parse Error", "expected "
					"object value but got ',' instead", str, pos);

			haveKey = false;
			haveValue = false;
			haveSemicolon = false;

			continue;
		}
		else if(QChar(':') == str[pos]) // Found the semicolon
		{
			if(!haveKey)
				throw JSONParseError("Object Parse Error", "expected "
					"object key but got ':' instead", str, pos);
			else if(haveKey && haveSemicolon && !haveValue)
				throw JSONParseError("Object Parse Error", "expected "
					"object value but got ':' instead", str, pos);
			else if(haveKey && haveValue)
				throw JSONParseError("Object Parse Error", "expected "
					"',' or '}' but got ':' instead", str, pos);

			haveSemicolon = true;

			continue;
		}

		if(haveKey) // Looking for the value
		{
			if(!haveSemicolon)
				throw JSONParseError("Object Parse Error", QString("expected "
					"':' but got '%1' instead").arg(str[pos]), str, pos);
			if(haveValue)
				throw JSONParseError("Object Parse Error", QString("expected "
					"',' or '}' but got '%1' instead").arg(str[pos]), str, pos);

			map[key] = parse_any();
			haveValue = true;
			pos -= 1;
		}
		else // Still need the key
		{
			key = parse_string().toString();
			haveKey = true;
			pos -= 1;

			if( key.isEmpty() )
				throw JSONParseError("Object Parse Error", "object key is an "
					"empty string", str + " ", len);
			if( map.contains(key) )
				throw JSONParseError("Object Parse Error", QString("duplicate "
					"key '%1' found").arg(key), str, pos - key.length() - 1);
		}
	}

	pos++;

	return QVariant(map);
}

QVariant json::parse_array()
{
	int len = str.length();

	if(len <= 0 || pos >= len)
		throw JSONParseError("Array Parse Error", "expected '[' but got the "
			"end of the stream instead", str + " ", len);

	if(QChar('[') != str[pos])
		throw JSONParseError("Array Parse Error", QString("expected '[' but "
			"got '%1' instead").arg(str[pos]), str, pos);

	QList<QVariant> list;
	bool haveItem = false;

	while(true)
	{
		pos++;

		// Skip any whitespace
		while( pos < len && isWhitespace(str[pos]) )
			pos++;

		if(pos >= len)
		{
			if(haveItem)
				throw JSONParseError("Array Parse Error", "expected ',' or ']' "
					"but got the end of the stream instead", str + " ", len);
			else if( !list.count() )
				throw JSONParseError("Array Parse Error", "expected array "
					"value or ']' but got the end of the stream "
					"instead", str + " ", len);
			else
				throw JSONParseError("Array Parse Error", "expected array value"
					" but got the end of the stream instead", str + " ", len);
		}

		if(QChar(']') == str[pos]) // Found the end of the array
		{
			if(list.count() && !haveItem)
				throw JSONParseError("Array Parse Error", "expected array "
					"value but got ']' instead", str, pos);

			break;
		}
		else if(QChar(',') == str[pos]) // Found a new element
		{
			if(!haveItem)
				throw JSONParseError("Array Parse Error", "expected array "
					"value but got ',' instead", str, pos);

			haveItem = false;
			continue;
		}

		if(haveItem)
			throw JSONParseError("Array Parse Error", QString("expected ',' or "
				"']' but got '%1' instead").arg(str[pos]), str, pos);

		// Add the array element
		list << parse_any();
		pos -= 1;

		haveItem = true;
	}

	pos++;

	return QVariant(list);
}

QVariant json::parse_string()
{
	int len = str.length();

	if(len <= 0 || pos >= len)
		throw JSONParseError("String Parse Error", "expected '[' but got the "
			"end of the stream instead", str + " ", len);

	if(QChar('"') != str[pos])
		throw JSONParseError("String Parse Error", QString("expected '\"' but "
			"got '%1' instead").arg(str[pos]), str, pos);

	pos++;
	int start = pos;
	int end = pos - 1;
	int index = -1;

	while( (index = str.indexOf(QChar('"'), end + 1)) != -1 )
	{
		end = index;
		if( str[end - 1] != QChar('\\') )
			break;
	}

	// End of the string not found
	if(index == -1)
		throw JSONParseError("String Parse Error", "expected '\"' "
			"but got the end of the stream instead", str + " ", len);

	// Empty String
	if(end == pos)
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
}

QVariant json::parse_null()
{
	int len = str.length();

	QString keyword("null");
	if(len <= 0 || (pos + keyword.length() - 1) >= len)
		throw JSONParseError("Null Parse Error", "expected 'null' but got the "
			"end of the stream instead", str + " ", len);

	if(str.mid(pos, keyword.length()).compare(keyword, Qt::CaseSensitive) != 0)
		throw JSONParseError("Null Parse Error",
			QString("expected 'null' but got '%1' instead").arg(
			str.mid(pos, keyword.length()) ), str, pos);

	pos += keyword.length();

	return QVariant();
}

QVariant json::parse_true()
{
	int len = str.length();

	QString keyword("true");
	if(len <= 0 || (pos + keyword.length() - 1) >= len)
		throw JSONParseError("True Parse Error", "expected 'true' but got the "
			"end of the stream instead", str + " ", len);

	if(str.mid(pos, keyword.length()).compare(keyword, Qt::CaseSensitive) != 0)
		throw JSONParseError("True Parse Error",
			QString("expected 'true' but got '%1' instead").arg(
			str.mid(pos, keyword.length()) ), str, pos);

	pos += keyword.length();

	return QVariant(true);
}

QVariant json::parse_false()
{
	int len = str.length();

	QString keyword("false");
	if(len <= 0 || (pos + keyword.length() - 1) >= len)
		throw JSONParseError("False Parse Error", "expected 'false' but got the "
			"end of the stream instead", str + " ", len);

	if(str.mid(pos, keyword.length()).compare(keyword, Qt::CaseSensitive) != 0)
		throw JSONParseError("False Parse Error",
			QString("expected 'false' but got '%1' instead").arg(
			str.mid(pos, keyword.length()) ), str, pos);

	pos += keyword.length();

	return QVariant(false);
}

QVariant json::parse_number()
{
	int len = str.length();

	QRegExp num_matcher("[0-9]");

	if(len <= 0 || pos >= len)
		throw JSONParseError("Number Parse Error", "expected '-' or a digit "
			"but got the end of the stream instead", str + " ", len);

	if( QChar('-') != str[pos] && !num_matcher.exactMatch(str.mid(pos, 1)) )
		throw JSONParseError("Number Parse Error", QString("expected '-' or a "
			"digit but got '%1' instead").arg(str[pos]), str, pos);

	QString num;
	int start = pos;

	// Negative sign
	if(QChar('-') == str[pos])
	{
		num = QChar('-');
		pos++;
	}

	if(pos >= len)
		throw JSONParseError("Number Parse Error", "expected a digit but got "
			"the end of the stream instead", str + " ", len);

	if( !num_matcher.exactMatch(str.mid(pos, 1)) )
		throw JSONParseError("Number Parse Error", QString("expected a digit "
			"but got '%1' instead").arg(str[pos]), str, pos);

	// Read in all digits
	while( num_matcher.exactMatch(str.mid(pos, 1)) )
	{
		num += str[pos];
		pos++;
	}

	if(QChar('.') == str[pos])
	{
		num += QChar('.');
		pos++;

		if(pos >= len)
			throw JSONParseError("Number Parse Error", "expected digit "
				"but got the end of the stream instead", str + " ", len);
		if( !num_matcher.exactMatch(str.mid(pos, 1)) )
			throw JSONParseError("Number Parse Error", QString("expected digit "
				"but got '%1' instead").arg(str[pos]), str, pos);

		while( num_matcher.exactMatch(str.mid(pos, 1)) )
		{
			num += str[pos];
			pos++;
		}
	}

	if(QChar('e') == str[pos] || QChar('E') == str[pos])
	{
		num += QChar('e');
		pos++;

		if(pos >= len)
			throw JSONParseError("Number Parse Error", "expected '+' or '-' or "
				"digit but got the end of the stream instead", str + " ", len);

		if( str[pos] == QChar('+') || str[pos] == QChar('-') )
		{
			num += str[pos];
			pos++;

			if(pos >= len)
				throw JSONParseError("Number Parse Error", "expected digit "
					"but got the end of the stream instead", str + " ", len);
			if( !num_matcher.exactMatch(str.mid(pos, 1)) )
				throw JSONParseError("Number Parse Error", QString("expected "
					"digit but got '%1' instead").arg(str[pos]), str, pos);
		}
		else if( !num_matcher.exactMatch(str.mid(pos, 1)) )
		{
			throw JSONParseError("Number Parse Error", QString("expected '+' or"
				" '-' or digit but got '%1' instead").arg(str[pos]), str, pos);
		}

		while( num_matcher.exactMatch(str.mid(pos, 1)) )
		{
			num += str[pos];
			pos++;
		}
	}

	if( num.contains('.') || num.contains('e') )
	{
		bool ok;
		float final = num.toFloat(&ok);
		if(!ok)
			throw JSONParseError("Number Parse Error",
				"unknown parse error", str, start);

		return final;
	}
	else
	{
		bool ok;
		int final = num.toInt(&ok);
		if(!ok)
			throw JSONParseError("Number Parse Error",
				"unknown parse error", str, start);

		return final;
	}
}
