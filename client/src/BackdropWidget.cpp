/******************************************************************************\
*  client/src/BackdropWidget.cpp                                               *
*  Copyright (C) 2009 John Eric Martin <john.eric.martin@gmail.com>            *
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

#include "BackdropWidget.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

BackdropWidget::BackdropWidget(QWidget *parent) : QWidget(parent)
{
	/// @TODO REMOVE THIS
	mBackdropPath = "icons/p3p/persona/PST_05A.png";
	mBackdropPath = "icons/p3p/persona/PST_08A.png";

	mBackdrop = QPixmap(mBackdropPath);
}

void BackdropWidget::setBackdrop(const QString& path)
{
	mBackdropPath = path;
	mBackdrop = QPixmap(path);

	// Clear the widget pixmap so it recreates it
	mWidgetPixmap = QPixmap();
}

void BackdropWidget::paintEvent(QPaintEvent *evt)
{
	if( !isVisible() || width() <= 0 || height() <= 0 )
		return;

	if( mWidgetPixmap.size() != size() )
	{
		mWidgetPixmap = QPixmap( size() );
		mWidgetPixmap.fill(Qt::transparent);

		int x = mWidgetPixmap.width() - mBackdrop.width();
		int y = mWidgetPixmap.height() - mBackdrop.height();

		QPainter painter(&mWidgetPixmap);
		painter.drawPixmap(x, y, mBackdrop);
	}

	QPainter painter(this);
	painter.fillRect(evt->rect(), Qt::transparent);
	painter.drawPixmap(evt->rect(), mWidgetPixmap);
}
