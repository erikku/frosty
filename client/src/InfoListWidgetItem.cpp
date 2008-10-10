/******************************************************************************\
*  client/src/InfoListWidgetItem.cpp                                           *
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

#include "InfoListWidgetItem.h"
#include "InfoListWidget.h"

#include <QtGui/QMouseEvent>

InfoListWidgetItem::InfoListWidgetItem(InfoListWidget *parent) :
	QWidget(0), mSelected(false)
{
	ui.setupUi(this);

	setMouseTracking(true);
	setAutoFillBackground(true);

	mUpperFont = ui.text1->font();
	mLowerFont = ui.text3->font();

	mInfoListWidget = parent;
	if(parent)
		parent->addItem(this);

	QPalette pal = palette();

	pal.setBrush(QPalette::Disabled, QPalette::HighlightedText,
		palette().brush(QPalette::Disabled, QPalette::WindowText));
	pal.setBrush(QPalette::Active, QPalette::HighlightedText,
		palette().brush(QPalette::Active, QPalette::WindowText));
	pal.setBrush(QPalette::Inactive, QPalette::HighlightedText,
		palette().brush(QPalette::Inactive, QPalette::WindowText));

	pal.setBrush(QPalette::Disabled, QPalette::Highlight,
		QBrush( QColor("#202020") ));
	pal.setBrush(QPalette::Active, QPalette::Highlight,
		QBrush( QColor("#404040") ));
	pal.setBrush(QPalette::Inactive, QPalette::Highlight,
		QBrush( QColor("#404040") ));

	setPalette(pal);
};

InfoListWidgetItem::InfoListWidgetItem(const QIcon& icon, const QString& text1,
	const QString& text2, const QString& text3, const QString& text4,
	InfoListWidget *parent) : QWidget(0), mSelected(false)
{
	ui.setupUi(this);

	setMouseTracking(true);
	setAutoFillBackground(true);

	mUpperFont = ui.text1->font();
	mLowerFont = ui.text3->font();

	ui.icon->setPixmap( icon.pixmap(32, 32) );
	ui.text1->setText(text1);
	ui.text2->setText(text2);
	ui.text3->setText(text3);
	ui.text4->setText(text4);

	mInfoListWidget = parent;
	if(parent)
		parent->addItem(this);

	QPalette pal = palette();

	pal.setBrush(QPalette::Disabled, QPalette::HighlightedText,
		palette().brush(QPalette::Disabled, QPalette::WindowText));
	pal.setBrush(QPalette::Active, QPalette::HighlightedText,
		palette().brush(QPalette::Active, QPalette::WindowText));
	pal.setBrush(QPalette::Inactive, QPalette::HighlightedText,
		palette().brush(QPalette::Inactive, QPalette::WindowText));

	pal.setBrush(QPalette::Disabled, QPalette::Highlight,
		QBrush( QColor("#202020") ));
	pal.setBrush(QPalette::Active, QPalette::Highlight,
		QBrush( QColor("#404040") ));
	pal.setBrush(QPalette::Inactive, QPalette::Highlight,
		QBrush( QColor("#404040") ));

	setPalette(pal);
};

void InfoListWidgetItem::mouseMoveEvent(QMouseEvent *event)
{
	event->accept();
	grabMouse();

	if(mInfoListWidget)
	{
		QWidget *viewport = mInfoListWidget->ui.listArea->viewport();
		QPoint bottom_left(viewport->width(), viewport->height());

		QRect area(viewport->mapToGlobal( QPoint(0, 0) ),
			viewport->mapToGlobal(bottom_left) );

		if( area.contains( event->globalPos() ) &&
			rect().contains( event->pos() ) )
		{
			setBackgroundRole(QPalette::Highlight);
		}
		else
		{
			releaseMouse();

			if(mSelected)
				setBackgroundRole(QPalette::AlternateBase);
			else
				setBackgroundRole(QPalette::NoRole);
		}
	}
	else
	{
		releaseMouse();
	}
};

void InfoListWidgetItem::mousePressEvent(QMouseEvent *event)
{
	if(!mInfoListWidget)
		return;

	QWidget *viewport = mInfoListWidget->ui.listArea->viewport();
	QPoint bottom_left(viewport->width(), viewport->height());

	QRect area(viewport->mapToGlobal( QPoint(0, 0) ),
		viewport->mapToGlobal(bottom_left) );

	if( area.contains( event->globalPos() ) && rect().contains( event->pos() ) )
	{
		window()->activateWindow();
		window()->raise();
	}
};

void InfoListWidgetItem::mouseReleaseEvent(QMouseEvent *event)
{
	if(!mInfoListWidget)
		return;

	QWidget *viewport = mInfoListWidget->ui.listArea->viewport();
	QPoint bottom_left(viewport->width(), viewport->height());

	QRect area(viewport->mapToGlobal( QPoint(0, 0) ),
		viewport->mapToGlobal(bottom_left) );

	if( area.contains( event->globalPos() ) && rect().contains( event->pos() ) )
	{
		event->accept();
		emit clicked();
	}
};

void InfoListWidgetItem::mouseDoubleClickEvent(QMouseEvent *event)
{
	if(!mInfoListWidget)
		return;

	QWidget *viewport = mInfoListWidget->ui.listArea->viewport();
	QPoint bottom_left(viewport->width(), viewport->height());

	QRect area(viewport->mapToGlobal( QPoint(0, 0) ),
		viewport->mapToGlobal(bottom_left) );

	if( area.contains( event->globalPos() ) && rect().contains( event->pos() ) )
	{
		event->accept();
		emit doubleClicked();
	}
};

bool InfoListWidgetItem::isSelected() const
{
	return mSelected;
};

void InfoListWidgetItem::setSelected(bool selected)
{
	if(mSelected == selected)
		return;

	mSelected = selected;

	if(mSelected)
		setBackgroundRole(QPalette::AlternateBase);
	else
		setBackgroundRole(QPalette::NoRole);

	emit selectionChanged();
};

QString InfoListWidgetItem::text1() const
{
	return ui.text1->text();
};

QString InfoListWidgetItem::text2() const
{
	return ui.text2->text();
};

QString InfoListWidgetItem::text3() const
{
	return ui.text3->text();
};

QString InfoListWidgetItem::text4() const
{
	return ui.text4->text();
};

QString InfoListWidgetItem::textAt(int index) const
{
	switch(index)
	{
		case 0:
			return text1();
			break;
		case 1:
			return text2();
			break;
		case 2:
			return text3();
			break;
		case 3:
			return text4();
			break;
		default:
			break;
	}

	return QString();
};

void InfoListWidgetItem::setText1(const QString& text)
{
	ui.text1->setText(text);
};

void InfoListWidgetItem::setText2(const QString& text)
{
	ui.text2->setText(text);
};

void InfoListWidgetItem::setText3(const QString& text)
{
	ui.text3->setText(text);
};

void InfoListWidgetItem::setText4(const QString& text)
{
	ui.text4->setText(text);
};

void InfoListWidgetItem::setTextAt(int index, const QString& text)
{
	switch(index)
	{
		case 0:
			return setText1(text);
			break;
		case 1:
			return setText2(text);
			break;
		case 2:
			return setText3(text);
			break;
		case 3:
			return setText4(text);
			break;
		default:
			break;
	}
};

QIcon InfoListWidgetItem::icon() const
{
	return QIcon( *ui.icon->pixmap() );
};

InfoListWidgetItem* InfoListWidgetItem::clone() const
{
	return new InfoListWidgetItem(icon(), text1(), text2(), text3(), text4());
};

InfoListWidget* InfoListWidgetItem::infoListWidget() const
{
	return mInfoListWidget;
};

void InfoListWidgetItem::setIcon(const QIcon& icon)
{
	ui.icon->setPixmap( icon.pixmap(32, 32) );
};

QVariant InfoListWidgetItem::data() const
{
	return mData;
};

void InfoListWidgetItem::setData(const QVariant& data)
{
	mData = data;
};

QFont InfoListWidgetItem::upperFont() const
{
	return mUpperFont;
};

QFont InfoListWidgetItem::lowerFont() const
{
	return mLowerFont;
};

void InfoListWidgetItem::setUpperFont(const QFont& font)
{
	mUpperFont = font;

	ui.text1->setFont(font);
	ui.text2->setFont(font);
};

void InfoListWidgetItem::setLowerFont(const QFont& font)
{
	mLowerFont = font;

	ui.text3->setFont(font);
	ui.text4->setFont(font);
};
