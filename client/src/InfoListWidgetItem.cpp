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

InfoListWidgetItem::InfoListWidgetItem(InfoListWidget *parent_widget) :
	QWidget(0), mSelected(false)
{
	ui.setupUi(this);

	setAutoFillBackground(true);

	mUpperFont = ui.text1->font();
	mLowerFont = ui.text3->font();

	mInfoListWidget = parent_widget;
	if(parent_widget)
		parent_widget->addItem(this);

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
}

InfoListWidgetItem::InfoListWidgetItem(const QIcon& ico, const QString& t1,
	const QString& t2, const QString& t3, const QString& t4,
	InfoListWidget *parent_widget) : QWidget(0), mSelected(false)
{
	ui.setupUi(this);

	setMouseTracking(true);
	setAutoFillBackground(true);

	mUpperFont = ui.text1->font();
	mLowerFont = ui.text3->font();

	ui.icon->setPixmap( ico.pixmap(32, 32) );
	ui.text1->setText(t1);
	ui.text2->setText(t2);
	ui.text3->setText(t3);
	ui.text4->setText(t4);

	mInfoListWidget = parent_widget;
	if(parent_widget)
		parent_widget->addItem(this);

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
}

void InfoListWidgetItem::enterEvent(QEvent *evt)
{
	if(!mInfoListWidget)
		return;

	evt->accept();
	setBackgroundRole(QPalette::Highlight);
}

void InfoListWidgetItem::leaveEvent(QEvent *evt)
{
	if(!mInfoListWidget)
		return;

	evt->accept();
	if(mSelected)
		setBackgroundRole(QPalette::AlternateBase);
	else
		setBackgroundRole(QPalette::NoRole);
}

void InfoListWidgetItem::mouseReleaseEvent(QMouseEvent *evt)
{
	if(!mInfoListWidget)
		return;

	evt->accept();
	emit clicked();
}

void InfoListWidgetItem::mouseDoubleClickEvent(QMouseEvent *evt)
{
	if(!mInfoListWidget)
		return;

	evt->accept();
	emit doubleClicked();
}

bool InfoListWidgetItem::isSelected() const
{
	return mSelected;
}

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
}

QString InfoListWidgetItem::text1() const
{
	return ui.text1->text();
}

QString InfoListWidgetItem::text2() const
{
	return ui.text2->text();
}

QString InfoListWidgetItem::text3() const
{
	return ui.text3->text();
}

QString InfoListWidgetItem::text4() const
{
	return ui.text4->text();
}

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
}

void InfoListWidgetItem::setText1(const QString& text)
{
	ui.text1->setText(text);
}

void InfoListWidgetItem::setText2(const QString& text)
{
	ui.text2->setText(text);
}

void InfoListWidgetItem::setText3(const QString& text)
{
	ui.text3->setText(text);
}

void InfoListWidgetItem::setText4(const QString& text)
{
	ui.text4->setText(text);
}

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
}

QIcon InfoListWidgetItem::icon() const
{
	return QIcon( *ui.icon->pixmap() );
}

InfoListWidgetItem* InfoListWidgetItem::clone() const
{
	InfoListWidgetItem *item = new InfoListWidgetItem(icon(), text1(),
		text2(), text3(), text4());
	item->setData( data() );

	return item;
}

InfoListWidget* InfoListWidgetItem::infoListWidget() const
{
	return mInfoListWidget;
}

void InfoListWidgetItem::setIcon(const QIcon& ico)
{
	ui.icon->setPixmap( ico.pixmap(32, 32) );
}

QVariant InfoListWidgetItem::data() const
{
	return mData;
}

void InfoListWidgetItem::setData(const QVariant& d)
{
	mData = d;
}

QFont InfoListWidgetItem::upperFont() const
{
	return mUpperFont;
}

QFont InfoListWidgetItem::lowerFont() const
{
	return mLowerFont;
}

void InfoListWidgetItem::setUpperFont(const QFont& fnt)
{
	mUpperFont = fnt;

	ui.text1->setFont(fnt);
	ui.text2->setFont(fnt);
}

void InfoListWidgetItem::setLowerFont(const QFont& fnt)
{
	mLowerFont = fnt;

	ui.text3->setFont(fnt);
	ui.text4->setFont(fnt);
}
