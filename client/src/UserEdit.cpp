/******************************************************************************\
*  client/src/UserEdit.cpp                                                     *
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

#include "UserEdit.h"

#include "ajax.h"
#include "sha1.h"

#include <QtGui/QMessageBox>

UserEdit::UserEdit(QWidget *parent_widget) : QWidget(parent_widget, Qt::Dialog),
	mHaveUser(false), mHavePerms(false), mHaveSalt(false)
{
	ui.setupUi(this);

	darkenWidget(ui.emailLabel);
	darkenWidget(ui.nameLabel);
	darkenWidget(ui.passwordLabel);
	darkenWidget(ui.permissionsLabel);

	setWindowModality(Qt::ApplicationModal);

	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(updateUser()));

	ajax::getSingletonPtr()->subscribe(this);
}

void UserEdit::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	qobject_cast<QLabel*>(widget)->setMargin(5);
	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
}

void UserEdit::showEdit(const QString& email)
{
	setWindowTitle( tr("%1 - Edit %2").arg(
		tr("Absolutely Frosty") ).arg(mEmail) );

	mEmail = email;
	refresh();
	show();
}

void UserEdit::clear()
{
	mHaveUser = false;
	mHavePerms = false;
	mHaveSalt = false;

	setEnabled(false);
	ui.emailEdit->clear();
	ui.nameEdit->clear();
	ui.passwordEdit->clear();
	ui.permissionsList->clear();
}

void UserEdit::refresh()
{
	clear();

	QVariantMap action;
	action["action"] = "auth_query_perms";
	action["email"] = mEmail;
	action["user_data"] = "auth_edit_query_perms";

	ajax::getSingletonPtr()->request(action);

	action["action"] = "auth_query_user";
	action["user_data"] = "auth_edit_query_user";

	ajax::getSingletonPtr()->request(action);

	action.clear();
	action["action"] = QString("salt");
	action["user_data"] = "salt";

	ajax::getSingletonPtr()->request(action);
}

void UserEdit::updateUser()
{
	QRegExp emailRegExp("^[a-zA-Z][\\w\\.-]*[a-zA-Z0-9]@[a-zA-Z0-9]"
		"[\\w\\.-]*[a-zA-Z0-9]\\.[a-zA-Z][a-zA-Z\\.]*[a-zA-Z]$");

	if( !emailRegExp.exactMatch( ui.emailEdit->text() ) )
	{
		QMessageBox::warning(this, tr("User Edit Error"), tr("You must enter a "
			"valid email address."));

		return;
	}

	QString pass = ui.passwordEdit->text();
	QRegExp passMatcher("[a-zA-Z0-9]{6,}");

	if( !pass.isEmpty() && !passMatcher.exactMatch(pass) )
	{
		QMessageBox::warning(this, tr("User Edit Error"), tr("Invalid password. "
			"Passwords can only contain a-z, A-Z, and 0-9 and must be at least 6 "
			"characters long"));

		return;
	}

	setEnabled(false);

	if( !pass.isEmpty() )
	{
		pass = mSalt + pass;

		unsigned char checksum[20];
		sha1((uchar*)pass.toAscii().data(), pass.toAscii().size(), checksum);

		pass = QString();
		for(int i = 0; i < 20; i++)
		{
			int byte = (int)checksum[i];
			pass += QString("%1").arg(byte, 2, 16, QLatin1Char('0'));
		}
	}

	QVariantMap action;
	action["action"] = QString("auth_modify_user");
	action["email"] = mEmail;
	action["new_email"] = ui.emailEdit->text();
	action["name"] = ui.nameEdit->text();

	if( !pass.isEmpty() )
		action["pass"] = pass;

	QVariantMap perms;
	for(int i = 0; i < ui.permissionsList->count(); i++)
	{
		perms[ui.permissionsList->item(i)->text()] =
			QVariant(ui.permissionsList->item(i)->checkState()
				== Qt::Checked ? true : false);
	}

	action["perms"] = perms;
	action["user_data"] = QString("auth_modify_user");

	ajax::getSingletonPtr()->request(action);
	setEnabled(false);
}

void UserEdit::ajaxResponse(const QVariantMap& resp, const QString& user_data)
{
	if(user_data == "auth_edit_query_perms")
	{
		if( resp.value("email") != mEmail )
			return;

		QVariantMap map = resp.value("perms").toMap();
		QMapIterator<QString, QVariant> i(map);
		while( i.hasNext() )
		{
			i.next();
			QListWidgetItem *item = new QListWidgetItem( i.key() );
			item->setCheckState(i.value().toBool() ?
				Qt::Checked : Qt::Unchecked);

			ui.permissionsList->addItem(item);
		}

		mHavePerms = true;
	}
	else if(user_data == "auth_edit_query_user")
	{
		QVariantMap map = resp.value("user").toMap();
		ui.emailEdit->setText( map.value("email").toString() );
		ui.nameEdit->setText( map.value("name").toString() );

		mHaveUser = true;
	}
	else if(user_data == "salt")
	{
		mSalt = resp.value("salt").toString();
		mHaveSalt = true;
	}
	else if(user_data == "auth_modify_user")
	{
		setWindowModality(Qt::NonModal);
		emit userChanged();
		close();
	}

	if(mHaveUser && mHavePerms && mHaveSalt)
		setEnabled(true);
}
