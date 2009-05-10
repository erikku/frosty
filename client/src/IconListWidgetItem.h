/******************************************************************************\
*  client/src/IconListWidgetItem.h                                             *
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

#ifndef __IconListWidgetItem_h__
#define __IconListWidgetItem_h__

#include "ui_IconListWidgetItem.h"

#include <QtGui/QIcon>
#include <QtGui/QFont>
#include <QtCore/QString>

class IconListWidget;

class IconListWidgetItem : public QWidget
{
	Q_OBJECT

	friend class IconListWidget;

public:
	IconListWidgetItem(IconListWidget *parent = 0);
	IconListWidgetItem(const QIcon& icon, const QString& upper_text = QString(),
		const QString& lower_text = QString(), IconListWidget *parent = 0);

	bool isSelected() const;
	void setSelected(bool selected);

	QVariant data() const;
	void setData(const QVariant& data);

	QIcon icon() const;
	QString upperText() const;
	QString lowerText() const;

	void setIcon(const QIcon& icon);
	void setUpperText(const QString& text);
	void setLowerText(const QString& text);

	QFont upperFont() const;
	QFont lowerFont() const;
	void setUpperFont(const QFont& font);
	void setLowerFont(const QFont& font);

	IconListWidgetItem* clone() const;

	IconListWidget* iconListWidget() const;

signals:
	void clicked();
	void doubleClicked();
	void selectionChanged();

protected:
	virtual void enterEvent(QEvent *event);
	virtual void leaveEvent(QEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);

	bool mSelected;
	QVariant mData;
	QFont mUpperFont, mLowerFont;
	IconListWidget *mIconListWidget;

	Ui::IconListWidgetItem ui;
};

#endif // __IconListWidgetItem_h__
