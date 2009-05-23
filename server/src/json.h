/******************************************************************************\
*  client/src/json.h                                                           *
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

#ifndef __json_h__
#define __json_h__

#include <QtCore/QString>
#include <QtCore/QVariant>

#ifdef QT_GUI_LIB
class QTreeWidgetItem;
#endif

class JSONParseError
{
public:
	JSONParseError(const QString& type, const QString& message,
		const QString& subject, int pos, int len = 1);

	QString type() const;
	QString message() const;
	QString subject() const;

	QString toHTML() const;
	QString toString() const;

	int from() const;
	int fromLine() const;

	int to() const;
	int toLine() const;

private:
	QString mType;
	QString mMessage;
	QString mSubject;

	int mPos, mLen;
	int mFrom, mTo;
	int mFromLine, mToLine;
};

class json
{
public:
	static QVariant parse(const QString& str, bool die = true);
	static QString toJSON(const QVariant& obj);

#ifdef QT_GUI_LIB
	static QList<QTreeWidgetItem*> toTree(const QVariant& obj);
#endif

protected:
	static bool isWhitespace(const QChar& c);

	static QString mapToJSON(const QVariant& obj);
	static QString listToJSON(const QVariant& obj);
	static QString stringToJSON(const QVariant& obj);

#ifdef QT_GUI_LIB
	static QList<QTreeWidgetItem*> mapToTree(const QVariant& obj);
	static QList<QTreeWidgetItem*> listToTree(const QVariant& obj);
#endif

	json(const QString& string);

	int pos;
	QString str;

	QVariant parse_any();

	QVariant parse_object();
	QVariant parse_array();
	QVariant parse_string();
	QVariant parse_null();
	QVariant parse_true();
	QVariant parse_false();
	QVariant parse_number();
};

#endif // __json_h__
