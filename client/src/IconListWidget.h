/******************************************************************************\
*  client/src/IconListWidget.h                                                 *
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

#ifndef __IconListWidget_h__
#define __IconListWidget_h__

#include <QtGui/QWidget>

class QLabel;
class QScrollArea;
class QVBoxLayout;
class IconListWidgetItem;

class IconListWidget : public QWidget
{
	Q_OBJECT

	friend class IconListWidgetItem;

public:
	IconListWidget(QWidget *parent = 0);

	typedef enum _SelectionMode
	{
		NoSelection = 0,
		SingleSelection,
		MultiSelection,
		ExtendedSelection,
		ContiguousSelection
	}SelectionMode;

	void addItem(IconListWidgetItem *item);
	void addItem(const QIcon& icon, const QString& upper_text,
		const QString& lower_text = QString());

	void insertItem(int row, IconListWidgetItem *item);
	void insertItem(int row, const QIcon& icon, const QString& upper_text,
		const QString& lower_text = QString());

	IconListWidgetItem* itemAt(int row) const;
	int row(IconListWidgetItem *item) const;

	QList<IconListWidgetItem*> selectedItems() const;

	void setCurrentRow(int row);
	void setCurrentItem(IconListWidgetItem *item);

	void removeItem(int row);
	void removeItem(IconListWidgetItem *item);

	IconListWidgetItem* takeItem(int row);
	IconListWidgetItem* takeItem(IconListWidgetItem *item);

	int count() const;

	SelectionMode selectionMode() const;
	void setSelectionMode(SelectionMode mode);

signals:
	void itemClicked(IconListWidgetItem *item);
	void itemDoubleClicked(IconListWidgetItem *item);
	void selectionChanged();

public slots:
	void clear();

protected slots:
	void handleItemClicked();
	void handleItemDoubleClicked();

protected:
	SelectionMode mSelectionMode;

	QVBoxLayout *mLayout;
	QScrollArea *mListArea;

	QList<IconListWidgetItem*> mItems;
};

#endif // __IconListWidget_h__
