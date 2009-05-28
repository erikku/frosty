/******************************************************************************\
*  client/src/UserList.cpp                                                     *
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

#include "UserList.h"
#include "UserEdit.h"

#include "ajax.h"

#include <QtGui/QListWidget>

UserList::UserList(QWidget *parent_widget) : QWidget(parent_widget)
{
	mUserEdit = new UserEdit;

	ui.setupUi(this);

	darkenWidget(ui.activeLabel);
	darkenWidget(ui.spacerLabel);
	darkenWidget(ui.inactiveLabel);

	ui.makeActiveButton->setIcon( QIcon(":/previous.png") );
	ui.makeInactiveButton->setIcon( QIcon(":/next.png") );

	ui.makeActiveButton->setEnabled(false);
	ui.makeInactiveButton->setEnabled(false);

	ui.activeEditButton->setEnabled(false);
	ui.inactiveEditButton->setEnabled(false);

	connect(mUserEdit, SIGNAL(userChanged()), this, SLOT(refresh()));

	connect(ui.activeList, SIGNAL(itemSelectionChanged()),
		this, SLOT(updateSelection()));

	connect(ui.inactiveList, SIGNAL(itemSelectionChanged()),
		this, SLOT(updateSelection()));

	connect(ui.closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	connect(ui.activeEditButton, SIGNAL(clicked(bool)),
		this, SLOT(editActiveUser()));

	connect(ui.inactiveEditButton, SIGNAL(clicked(bool)),
		this, SLOT(editInactiveUser()));

	connect(ui.makeActiveButton, SIGNAL(clicked(bool)),
		this, SLOT(makeUserActive()));

	connect(ui.makeInactiveButton, SIGNAL(clicked(bool)),
		this, SLOT(makeUserInactive()));

	setWindowTitle( tr("%1 - User List").arg(
		tr("Absolutely Frosty") ) );

	ajax::getSingletonPtr()->subscribe(this);
}

void UserList::makeUserActive()
{
	if( ui.inactiveList->selectedItems().isEmpty() )
		return;

	QListWidgetItem *item = ui.inactiveList->selectedItems().first();

	QVariantMap action;
	action["action"] = QString("auth_make_active");
	action["email"] = item->data(Qt::UserRole);
	action["user_data"] = QString("auth_make_active");

	ajax::getSingletonPtr()->request(action);

	clear();
}

void UserList::makeUserInactive()
{
	if( ui.activeList->selectedItems().isEmpty() )
		return;

	QListWidgetItem *item = ui.activeList->selectedItems().first();

	QVariantMap action;
	action["action"] = QString("auth_make_inactive");
	action["email"] = item->data(Qt::UserRole);
	action["user_data"] = QString("auth_make_inactive");

	ajax::getSingletonPtr()->request(action);

	clear();
}

void UserList::updateSelection()
{
	ui.makeActiveButton->setEnabled( ui.inactiveList->selectedItems().count() );
	ui.makeInactiveButton->setEnabled( ui.activeList->selectedItems().count() );

	ui.activeEditButton->setEnabled( ui.activeList->selectedItems().count() );
	ui.inactiveEditButton->setEnabled( ui.inactiveList->selectedItems().count() );
}

void UserList::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	qobject_cast<QLabel*>(widget)->setMargin(5);
	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
}

void UserList::clear()
{
	ui.activeList->clear();
	ui.inactiveList->clear();

	ui.activeList->setEnabled(false);
	ui.inactiveList->setEnabled(false);
}

void UserList::editActiveUser()
{
	if( ui.activeList->selectedItems().isEmpty() )
		return;

	QListWidgetItem *item = ui.activeList->selectedItems().first();

	mUserEdit->showEdit( item->data(Qt::UserRole).toString() );
}

void UserList::editInactiveUser()
{
	if( ui.inactiveList->selectedItems().isEmpty() )
		return;

	QListWidgetItem *item = ui.inactiveList->selectedItems().first();

	mUserEdit->showEdit( item->data(Qt::UserRole).toString() );
}

void UserList::refresh()
{
	clear();

	QVariantMap action;
	action["action"] = "auth_query_users";
	action["user_data"] = "auth_query_users";

	ajax::getSingletonPtr()->request(action);
}

void UserList::ajaxResponse(const QVariantMap& resp, const QString& user_data)
{
	if(user_data == "auth_query_users")
	{
		QVariantList users = resp.value("users").toList();
		for(int i = 0; i < users.count(); i++)
		{
			QVariantMap user = users.at(i).toMap();

			QListWidgetItem *item = new QListWidgetItem( user.value("name").toString() );
			item->setData(Qt::UserRole, user.value("email"));

			if( user.value("active").toBool() )
				ui.activeList->addItem(item);
			else
				ui.inactiveList->addItem(item);
		}

		ui.activeList->setEnabled(true);
		ui.inactiveList->setEnabled(true);
	}
	else if(user_data == "auth_make_active" ||
		user_data == "auth_make_inactive")
	{
		refresh();
	}
}
