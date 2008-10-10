/******************************************************************************\
*  client/src/InfoListWidget.h                                                 *
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

#ifndef __InfoListWidget_h__
#define __InfoListWidget_h__

#include "ui_InfoListWidget.h"

class InfoListWidgetItem;

typedef bool (*InfoItemSortFunc)
	(const InfoListWidgetItem*, const InfoListWidgetItem*);

class InfoListWidget : public QWidget
{
	Q_OBJECT

	friend class InfoListWidgetItem;

public:
	InfoListWidget(QWidget *parent = 0);

	typedef enum _SelectionMode
	{
		NoSelection = 0,
		SingleSelection,
		MultiSelection,
		ExtendedSelection,
		ContiguousSelection,
	}SelectionMode;

	void addItem(InfoListWidgetItem *item);
	void addItem(const QIcon& icon, const QString& text1,
		const QString& text2 = QString(), const QString& text3 = QString(),
		const QString& text4 = QString());

	void insertItem(int row, InfoListWidgetItem *item);
	void insertItem(int row, const QIcon& icon, const QString& text1,
		const QString& text2 = QString(), const QString& text3 = QString(),
		const QString& text4 = QString());

	InfoListWidgetItem* item(int row) const;
	int row(const InfoListWidgetItem *item) const;

	QList<InfoListWidgetItem*> selectedItems() const;

	void setCurrentRow(int row);
	void setCurrentItem(InfoListWidgetItem *item);

	void removeItem(int row);
	void removeItem(InfoListWidgetItem *item);

	InfoListWidgetItem* takeItem(int row);
	InfoListWidgetItem* takeItem(InfoListWidgetItem *item);

	int count() const;

	bool isSortingEnabled() const;
	void setSortingEnabled(bool enable);

	SelectionMode selectionMode() const;
	void setSelectionMode(SelectionMode mode);

	void setSortHeaders(const QString& name1, const QString& name2 = QString(),
		const QString& name3 = QString(), const QString& name4 = QString());

	void setSortFunction1(InfoItemSortFunc func);
	void setSortFunction2(InfoItemSortFunc func);
	void setSortFunction3(InfoItemSortFunc func);
	void setSortFunction4(InfoItemSortFunc func);

signals:
	void itemClicked(InfoListWidgetItem *item);
	void itemDoubleClicked(InfoListWidgetItem *item);
	void selectionChanged(InfoListWidgetItem *item);

public slots:
	void sort();
	void clear();

	void toggleSort1();
	void toggleSort2();
	void toggleSort3();
	void toggleSort4();
	void toggleSort(int num);

protected slots:
	void handleItemClicked();
	void handleItemDoubleClicked();
	void handleSelectionChanged();

protected:
	QLabel* arrowAt(int index);

	bool lessThen(const InfoListWidgetItem *item1,
		const InfoListWidgetItem *item2);

	void mergeSort(QList<InfoListWidgetItem*>& a);

	Ui::InfoListWidget ui;
	SelectionMode mSelectionMode;

	bool mSortingEnabled;
	QVBoxLayout *mLayout;
	QList<int> mSortOrder;
	QList<int> mSortDirection;
	QList<InfoListWidgetItem*> mItems;
	QList<InfoItemSortFunc> mSortFunctions;
	QPixmap mIconUp1, mIconUp2, mIconDown1, mIconDown2;
};

#endif // __InfoListWidget_h__
