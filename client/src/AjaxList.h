/******************************************************************************\
*  client/src/AjaxList.h                                                       *
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

#ifndef __AjaxList_h__
#define __AjaxList_h__

#include "ui_AjaxList.h"

#include <QtCore/QMap>
#include <QtCore/QVariant>

class AjaxView;
class QListWidgetItem;

class AjaxList : public QWidget
{
	Q_OBJECT

	friend class AjaxView;

public:
	AjaxList(AjaxView *view = 0, QWidget *parent = 0);

public slots:
	void refresh();
	void updateFilter();
	void updateSearch();
	void deleteAjax();

protected slots:
	void ajaxResponse(const QVariantMap& resp, const QString& user_data);
	void handleItemInserted(int id);
	void handleItemSelected();

signals:
	void addItemRequested();
	void itemClicked(int id);

protected:
	virtual QVariant filterAction() const = 0;
	virtual QVariant filterUserData() const = 0;

	virtual QVariant listAction() const = 0;
	virtual QVariant listUserData() const = 0;

	virtual QString switchTitle() const = 0;
	virtual QString switchMessage() const = 0;

	virtual QString deleteTitle() const = 0;
	virtual QString deleteMessage() const = 0;
	virtual QVariant deleteAction(int id) const = 0;
	virtual QVariant deleteUserData() const = 0;

	virtual int itemID(const QVariantMap& map) const;
	virtual int filterID(const QVariantMap& map) const = 0;
	virtual QString itemText(const QVariantMap& map) const;
	virtual QString itemIcon(const QVariantMap& map) const = 0;
	virtual bool itemMatches(const QVariantMap& map, const QString& search);

	virtual bool hasIcon() const;

	Ui::AjaxList ui;

	int mFilterID;
	int mCurrentID;

	int mDataLoaded;

	AjaxView *mAjaxView;
	QVariantList mItems;
	QListWidgetItem *mLastItem;
};

#endif // __AjaxList_h__
