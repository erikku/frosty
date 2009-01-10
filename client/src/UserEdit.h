/******************************************************************************\
*  client/src/UserEdit.h                                                       *
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

#ifndef __UserEdit_h__
#define __UserEdit_h__

#include "ui_UserEdit.h"

class UserEdit : public QWidget
{
	Q_OBJECT

public:
	UserEdit(QWidget *parent = 0);

public slots:
	void clear();
	void refresh();
	void updateUser();
	void showEdit(const QString& email);

protected slots:
	void ajaxResponse(const QVariantMap& resp, const QString& user_data);

signals:
	void userChanged();

protected:
	void darkenWidget(QWidget *widget);

	bool mHaveUser, mHavePerms, mHaveSalt;

	QString mEmail, mSalt;

	Ui::UserEdit ui;
};

#endif // __UserEdit_h__
