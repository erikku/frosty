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

SearchEdit::SearchEdit(QWidget *parent_widget) : QLineEdit(parent_widget)
{
	QPalette pal = palette();
	pal.setBrush(QPalette::Active, QPalette::Text,
		pal.brush(QPalette::Inactive, QPalette::Text));
	pal.setBrush(QPalette::Normal, QPalette::Text,
		pal.brush(QPalette::Inactive, QPalette::Text));

	mNoticePalette = pal;
	mNormalPalette = palette();

	mNotice = true;
	mNoticeText = tr("Enter search here.");
	setToolTip(mNoticeText);

	setPalette(mNoticePalette);
	QLineEdit::setText(mNoticeText);
}

SearchEdit::SearchEdit(const QString& notice,
	QWidget *parent_widget) : QLineEdit(parent_widget)
{
	QPalette pal = palette();
	pal.setBrush(QPalette::Active, QPalette::Text,
		pal.brush(QPalette::Inactive, QPalette::Text));
	pal.setBrush(QPalette::Normal, QPalette::Text,
		pal.brush(QPalette::Inactive, QPalette::Text));

	mNoticePalette = pal;
	mNormalPalette = palette();

	mNotice = true;
	mNoticeText = notice;
	setToolTip(notice);

	setPalette(mNoticePalette);
	QLineEdit::setText(mNoticeText);
}

QString SearchEdit::text() const
{
	if( QLineEdit::text() == mNoticeText )
		return QString();

	return QLineEdit::text();
}

QString SearchEdit::noticeText() const
{
	return mNoticeText;
}

void SearchEdit::setNoticeText(const QString& txt)
{
	mNoticeText = txt;
	setToolTip(txt);
}

QPalette SearchEdit::normalPalette() const
{
	return mNormalPalette;
}

void SearchEdit::setNormalPalette(const QPalette& pal)
{
	mNormalPalette = pal;
}

QPalette SearchEdit::noticePalette() const
{
	return mNoticePalette;
}

void SearchEdit::setNoticePalette(const QPalette& pal)
{
	mNoticePalette = pal;
}

void SearchEdit::focusInEvent(QFocusEvent *evt)
{
	setPalette(mNormalPalette);

	if(mNotice)
		QLineEdit::setText( QString() );

	QLineEdit::focusInEvent(evt);
}

void SearchEdit::focusOutEvent(QFocusEvent *evt)
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

	QLineEdit::focusOutEvent(evt);
}

void SearchEdit::setText(const QString& txt)
{
	if( txt.isEmpty() )
		return clear();

	QLineEdit::setText(txt);

	mNotice = false;
	setPalette(mNormalPalette);
}

void SearchEdit::dragEnterEvent(QDragEnterEvent *evt)
{
	setPalette(mNormalPalette);

	if(mNotice)
		QLineEdit::setText( QString() );

	QLineEdit::dragEnterEvent(evt);
}

void SearchEdit::dragLeaveEvent(QDragLeaveEvent *evt)
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

	QLineEdit::dragLeaveEvent(evt);
}

void SearchEdit::dropEvent(QDropEvent *evt)
{
	QLineEdit::dropEvent(evt);

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
}

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
}
