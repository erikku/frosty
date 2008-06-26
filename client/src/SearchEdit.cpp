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

#include "SearchEdit.h"

SearchEdit::SearchEdit(QWidget *parent) : QLineEdit(parent)
{
	QPalette pal = palette();
	pal.setBrush( QPalette::Active, QPalette::Text, pal.brush(QPalette::Inactive, QPalette::Text) );
	pal.setBrush( QPalette::Normal, QPalette::Text, pal.brush(QPalette::Inactive, QPalette::Text) );

	mNoticePalette = pal;
	mNormalPalette = palette();

	mNotice = true;
	mNoticeText = tr("Enter search here.");
	setToolTip(mNoticeText);

	setPalette(mNoticePalette);
	QLineEdit::setText(mNoticeText);
};

SearchEdit::SearchEdit(const QString& notice, QWidget *parent) : QLineEdit(parent)
{
	QPalette pal = palette();
	pal.setBrush( QPalette::Active, QPalette::Text, pal.brush(QPalette::Inactive, QPalette::Text) );
	pal.setBrush( QPalette::Normal, QPalette::Text, pal.brush(QPalette::Inactive, QPalette::Text) );

	mNoticePalette = pal;
	mNormalPalette = palette();

	mNotice = true;
	mNoticeText = notice;
	setToolTip(notice);

	setPalette(mNoticePalette);
	QLineEdit::setText(mNoticeText);
};

QString SearchEdit::text() const
{
	if( QLineEdit::text() == mNoticeText )
		return QString();

	return QLineEdit::text();
}

QString SearchEdit::noticeText() const
{
	return mNoticeText;
};

void SearchEdit::setNoticeText(const QString& text)
{
	mNoticeText = text;
	setToolTip(mNoticeText);
};

QPalette SearchEdit::normalPalette() const
{
	return mNormalPalette;
};

void SearchEdit::setNormalPalette(const QPalette& palette)
{
	mNormalPalette = palette;
};

QPalette SearchEdit::noticePalette() const
{
	return mNoticePalette;
};

void SearchEdit::setNoticePalette(const QPalette& palette)
{
	mNoticePalette = palette;
};

void SearchEdit::focusInEvent(QFocusEvent *event)
{
	setPalette(mNormalPalette);

	if(mNotice)
		QLineEdit::setText( QString() );

	QLineEdit::focusInEvent(event);
};
void SearchEdit::focusOutEvent(QFocusEvent *event)
{
	mNotice = text().isEmpty();
	if(mNotice)
	{
		setPalette(mNoticePalette);
		QLineEdit::setText(mNoticeText);
	}
	else
	{
		setPalette(mNormalPalette);
	}

	QLineEdit::focusOutEvent(event);
};

void SearchEdit::setText(const QString& text)
{
	if( text.isEmpty() )
		return clear();

	QLineEdit::setText(text);

	mNotice = false;
	setPalette(mNormalPalette);
};

void SearchEdit::dragEnterEvent(QDragEnterEvent *event)
{
	setPalette(mNormalPalette);

	if(mNotice)
		QLineEdit::setText( QString() );

	QLineEdit::dragEnterEvent(event);
};

void SearchEdit::dragLeaveEvent(QDragLeaveEvent *event)
{
	mNotice = text().isEmpty();
	if(mNotice)
	{
		setPalette(mNoticePalette);
		QLineEdit::setText(mNoticeText);
	}
	else
	{
		setPalette(mNormalPalette);
	}

	QLineEdit::dragLeaveEvent(event);
};

void SearchEdit::dropEvent(QDropEvent *event)
{
	QLineEdit::dropEvent(event);

	mNotice = text().isEmpty();
	if(mNotice)
	{
		setPalette(mNoticePalette);
		QLineEdit::setText(mNoticeText);
	}
	else
	{
		setPalette(mNormalPalette);
	}
};

void SearchEdit::clear()
{
	QLineEdit::clear();

	if(hasFocus())
	{
		mNotice = false;
		setPalette(mNormalPalette);
	}
	else
	{
		mNotice = true;
		setPalette(mNoticePalette);
		QLineEdit::setText(mNoticeText);
	}
};
