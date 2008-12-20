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

Shoutbox::Shoutbox(QWidget *parent_widget) : QWidget(parent_widget),
	mLastStamp(0)
{
	ui.setupUi(this);

	setWindowTitle( tr("%1 - Shoutbox").arg(
		tr("Absolutely Frosty") ) );

	ajax::getSingletonPtr()->subscribe(this);

	mTimer = new QTimer;
	mTimer->start(5000);

	connect(mTimer, SIGNAL(timeout()), this, SLOT(checkNew()));
	connect(ui.shoutButton, SIGNAL(clicked(bool)), this, SLOT(shout()));
	connect(ui.messageEdit, SIGNAL(returnPressed()), this, SLOT(shout()));
	connect(ui.messageEdit, SIGNAL(textChanged(const QString&)),
		this, SLOT(updateShoutButton()));

	ui.chatLog->insertPlainText( tr("-- Start Shoutbox Log --") );

	updateShoutButton();
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

void Shoutbox::ajaxResponse(const QVariant& resp)
{
	QVariantMap result = resp.toMap();

	if(result.value("user_data").toString() == "shoutbox")
	{
		QVariantList msgs = result.value("messages").toList();
		foreach(QVariant msg_item, msgs)
		{
			QVariantMap msg = msg_item.toMap();

			ui.chatLog->insertHtml("<br/><b>" +
				msg.value("author").toString() + QString(":</b> ") +
				msg.value("text").toString() );
		}

		// Set the new timestamp
		mLastStamp = result.value("timestamp").toUInt();

		if( !msgs.isEmpty() )
			ui.chatLog->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	}
}
