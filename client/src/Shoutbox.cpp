/******************************************************************************\
*  client/src/Shoutbox.cpp                                                     *
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

#include "Shoutbox.h"
#include "Settings.h"
#include "ajax.h"

#include <QtCore/QTimer>
#include <QtCore/QDateTime>

Shoutbox::Shoutbox(QWidget *parent_widget) : QWidget(parent_widget),
	mAdmin(false), mModify(false), mLastStamp(0)
{
	ui.setupUi(this);

	setWindowTitle( tr("%1 - Shoutbox").arg(
		tr("Absolutely Frosty") ) );

	ajax::getSingletonPtr()->subscribe(this);

	mTimer = new QTimer;
	mNick = tr("me");

	connect(mTimer, SIGNAL(timeout()), this, SLOT(checkNew()));
	connect(ui.shoutButton, SIGNAL(clicked(bool)), this, SLOT(shout()));
	connect(ui.messageEdit, SIGNAL(returnPressed()), this, SLOT(shout()));
	connect(ui.messageEdit, SIGNAL(textChanged(const QString&)),
		this, SLOT(updateShoutButton()));

	ui.chatLog->insertPlainText( tr("-- Start Shoutbox Log --") );

	updateShoutButton();

	QVariantMap action;
	action["action"] = "shoutbox_login";
	action["timestamp"] = mLastStamp;
	action["user_data"] = "shoutbox_login";

	ajax::getSingletonPtr()->request(settings->url(), action);

	ui.messageEdit->setEnabled(false);
}

void Shoutbox::shout()
{
	if( ui.messageEdit->text().isEmpty() )
		return;

	QVariantMap action;
	action["action"] = "shoutbox_post";
	action["text"] = ui.messageEdit->text();
	action["user_data"] = "shoutbox";

	ajax::getSingletonPtr()->request(settings->url(), action);

	QString stamp = QDateTime::currentDateTime().toString(
		"MM/dd hh:mm:ss");

	ui.chatLog->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);

	if(mAdmin)
	{
		// #99FF00 - System Message Green
		ui.chatLog->insertHtml( tr("<br/><b>(%1) <font color=#FF3300>"
			"%2:</font></b> <font color=#FF3300>%3</font>").arg(
			stamp).arg(mNick).arg( ui.messageEdit->text() ) );
	}
	else if(mModify)
	{
		ui.chatLog->insertHtml( tr("<br/><b>(%1) <font color=#0099FF>"
			"%2:</font></b> <font color=#0099FF>%3</font>").arg(
			stamp).arg(mNick).arg( ui.messageEdit->text() ) );
	}
	else
	{
		ui.chatLog->insertHtml( tr("<br/><b>(%1) %2:</b> %3").arg(
			stamp).arg(mNick).arg( ui.messageEdit->text() ) );
	}

	ui.chatLog->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);

	ui.messageEdit->clear();
}

void Shoutbox::checkNew()
{
	if( !isVisible() )
		return;

	QVariantMap action;
	action["action"] = "shoutbox_poll";
	action["timestamp"] = mLastStamp;
	action["user_data"] = "shoutbox";

	ajax::getSingletonPtr()->request(settings->url(), action);
}

void Shoutbox::updateShoutButton()
{
	ui.shoutButton->setEnabled( !ui.messageEdit->text().isEmpty() );
}

void Shoutbox::ajaxResponse(const QVariantMap& resp, const QString& user_data)
{
	if(user_data == "shoutbox")
	{
		QVariantList msgs = resp.value("messages").toList();
		foreach(QVariant msg_item, msgs)
		{
			QVariantMap msg = msg_item.toMap();

			QString stamp = QDateTime::fromTime_t( msg.value(
				"timestamp").toUInt() ).toString("MM/dd hh:mm:ss");

			ui.chatLog->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
			if( msg.value("admin").toBool() )
			{
				// #99FF00 - System Message Green
				ui.chatLog->insertHtml( tr("<br/><b>(%1) <font color=#FF3300>"
					"%2:</font></b> <font color=#FF3300>%3</font>").arg(
					stamp).arg( msg.value("author").toString() ).arg(
					msg.value("text").toString() ) );
			}
			else if( msg.value("modify_db").toBool() )
			{
				ui.chatLog->insertHtml( tr("<br/><b>(%1) <font color=#0099FF>"
					"%2:</font></b> <font color=#0099FF>%3</font>").arg(
					stamp).arg( msg.value("author").toString() ).arg(
					msg.value("text").toString() ) );
			}
			else
			{
				ui.chatLog->insertHtml( tr("<br/><b>(%1) %2:</b> %3").arg(
					stamp).arg( msg.value("author").toString() ).arg(
					msg.value("text").toString() ) );
			}
		}

		// Set the new timestamp
		if( resp.contains("timestamp") )
			mLastStamp = resp.value("timestamp").toUInt();

		if( !msgs.isEmpty() )
			ui.chatLog->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	}

	if(user_data == "shoutbox_login")
	{
		mNick = resp.value("nick").toString();
		mAdmin = resp.value("admin").toBool();
		mModify = resp.value("modify_db").toBool();

		mTimer->start(5000);

		ui.messageEdit->setEnabled(true);
	}
}
