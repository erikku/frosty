/******************************************************************************\
*  Utopia Player - A cross-platform, multilingual, tagging media manager       *
*  Copyright (C) 2006-2007 John Eric Martin <john.eric.martin@gmail.com>       *
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

#ifndef __SearchEdit_h__
#define __SearchEdit_h__

#include <QtGui/QPalette>
#include <QtGui/QLineEdit>

class SearchEdit : public QLineEdit
{
	Q_OBJECT

public:
	SearchEdit(QWidget *parent = 0);
	SearchEdit(const QString& notice, QWidget *parent = 0);

	QString text() const;

	QString noticeText() const;
	QPalette normalPalette() const;
	QPalette noticePalette() const;

public slots:
	void clear();

	void setText(const QString& text);
	void setNoticeText(const QString& text);
	void setNormalPalette(const QPalette& palette);
	void setNoticePalette(const QPalette& palette);

protected:
	virtual void focusInEvent(QFocusEvent *event);
	virtual void focusOutEvent(QFocusEvent *event);
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dragLeaveEvent(QDragLeaveEvent *event);
	virtual void dropEvent(QDropEvent *event);

	bool mNotice;
	QString mNoticeText;
	QPalette mNoticePalette;
	QPalette mNormalPalette;
};

#endif // __SearchEdit_h__
