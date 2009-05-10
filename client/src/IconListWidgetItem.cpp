/******************************************************************************\
*  client/src/IconListWidgetItem.cpp                                           *
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

#include "IconListWidgetItem.h"
#include "IconListWidget.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QScrollArea>

IconListWidgetItem::IconListWidgetItem(IconListWidget *parent_widget) :
	QWidget(0), mSelected(false)
{
	ui.setupUi(this);

	setAutoFillBackground(true);

	mUpperFont = ui.text1->font();
	mLowerFont = ui.text2->font();

	mIconListWidget = parent_widget;
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

IconListWidgetItem::IconListWidgetItem(const QIcon& ico, const QString& t1,
	const QString& t2, IconListWidget *parent_widget) : QWidget(0),
	mSelected(false)
{
	ui.setupUi(this);

	setMouseTracking(true);
	setAutoFillBackground(true);

	mUpperFont = ui.text1->font();
	mLowerFont = ui.text2->font();

	ui.icon->setPixmap( ico.pixmap(32, 32) );
	setUpperText(t1);
	setLowerText(t2);

	mIconListWidget = parent_widget;
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

void IconListWidgetItem::enterEvent(QEvent *evt)
{
	if(!mIconListWidget)
		return;

	evt->accept();
	setBackgroundRole(QPalette::Highlight);
}

void IconListWidgetItem::leaveEvent(QEvent *evt)
{
	if(!mIconListWidget)
		return;

	evt->accept();
	if(mSelected)
		setBackgroundRole(QPalette::AlternateBase);
	else
		setBackgroundRole(QPalette::NoRole);
}

void IconListWidgetItem::mouseReleaseEvent(QMouseEvent *evt)
{
	if(!mIconListWidget)
		return;

	evt->accept();
	emit clicked();
}

void IconListWidgetItem::mouseDoubleClickEvent(QMouseEvent *evt)
{
	if(!mIconListWidget)
		return;

	evt->accept();
	emit doubleClicked();
}

bool IconListWidgetItem::isSelected() const
{
	return mSelected;
}

void IconListWidgetItem::setSelected(bool selected)
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

QString IconListWidgetItem::upperText() const
{
	return ui.text1->text();
}

QString IconListWidgetItem::lowerText() const
{
	return ui.text2->text();
}

void IconListWidgetItem::setUpperText(const QString& text)
{
	ui.text1->setText(text);
}

void IconListWidgetItem::setLowerText(const QString& text)
{
	ui.text2->setText(text);
}

QIcon IconListWidgetItem::icon() const
{
	return QIcon( *ui.icon->pixmap() );
}

IconListWidgetItem* IconListWidgetItem::clone() const
{
	IconListWidgetItem *item = new IconListWidgetItem(icon(),
		upperText(), lowerText());
	item->setData( data() );

	return item;
}

IconListWidget* IconListWidgetItem::iconListWidget() const
{
	return mIconListWidget;
}

void IconListWidgetItem::setIcon(const QIcon& ico)
{
	ui.icon->setPixmap( ico.pixmap(32, 32) );
}

QVariant IconListWidgetItem::data() const
{
	return mData;
}

void IconListWidgetItem::setData(const QVariant& d)
{
	mData = d;
}

QFont IconListWidgetItem::upperFont() const
{
	return mUpperFont;
}

QFont IconListWidgetItem::lowerFont() const
{
	return mLowerFont;
}

void IconListWidgetItem::setUpperFont(const QFont& fnt)
{
	mUpperFont = fnt;

	ui.text1->setFont(fnt);
}

void IconListWidgetItem::setLowerFont(const QFont& fnt)
{
	mLowerFont = fnt;

	ui.text2->setFont(fnt);
}
