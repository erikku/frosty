/******************************************************************************\
*  client/src/UserList.h                                                       *
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

#ifndef __UserList_h__
#define __UserList_h__

#include "ui_UserList.h"

#include <QtCore/QVariant>
#include <QtCore/QMap>

class UserEdit;

class UserList : public QWidget
{
	Q_OBJECT

public:
	UserList(QWidget *parent = 0);

public slots:
	void refresh();
	void updateSelection();

protected slots:
	void clear();
	void editActiveUser();
	void editInactiveUser();
	void makeUserActive();
	void makeUserInactive();
	void ajaxResponse(const QVariantMap& resp, const QString& user_data);

protected:
	void darkenWidget(QWidget *widget);

	UserEdit *mUserEdit;

	Ui::UserList ui;
};

#endif // __UserList_h__
