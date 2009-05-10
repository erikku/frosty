/******************************************************************************\
*  client/src/InfoListWidgetItem.h                                             *
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

#ifndef __InfoListWidgetItem_h__
#define __InfoListWidgetItem_h__

#include "ui_InfoListWidgetItem.h"

#include <QtGui/QIcon>
#include <QtGui/QFont>
#include <QtCore/QString>

class InfoListWidget;

class InfoListWidgetItem : public QWidget
{
	Q_OBJECT

	friend class InfoListWidget;

public:
	InfoListWidgetItem(InfoListWidget *parent = 0);
	InfoListWidgetItem(const QIcon& icon, const QString& text1,
		const QString& text2 = QString(), const QString& text3 = QString(),
		const QString& text4 = QString(), InfoListWidget *parent = 0);

	bool isSelected() const;
	void setSelected(bool selected);

	QVariant data() const;
	void setData(const QVariant& data);

	QIcon icon() const;
	QString text1() const;
	QString text2() const;
	QString text3() const;
	QString text4() const;
	QString textAt(int index = 0) const;

	void setIcon(const QIcon& icon);
	void setText1(const QString& text);
	void setText2(const QString& text);
	void setText3(const QString& text);
	void setText4(const QString& text);
	void setTextAt(int index, const QString& text);

	QFont upperFont() const;
	QFont lowerFont() const;
	void setUpperFont(const QFont& font);
	void setLowerFont(const QFont& font);

	InfoListWidgetItem* clone() const;

	InfoListWidget* infoListWidget() const;

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
	InfoListWidget *mInfoListWidget;

	Ui::InfoListWidgetItem ui;
};

#endif // __InfoListWidgetItem_h__
