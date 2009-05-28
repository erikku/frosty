/******************************************************************************\
*  client/src/Instance.cpp                                                     *
*  Copyright (C) 2009 John Eric Martin <john.eric.martin@gmail.com>            *
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

#include "Instance.h"

#include "VersionCheck.h"
#include "IconImport.h"
#include "Settings.h"
#include "Register.h"
#include "Taskbar.h"

#include <QtCore/QTimer>
#include <QtGui/QApplication>

#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>

static Instance *g_instance_inst = 0;

Instance::Instance(QObject *parent) : QObject(parent)
{
	g_instance_inst = this;

	mLoginOK = false;

	mSocket = new QLocalSocket;
	mSocket->connectToServer("frosty_instance");

	QTimer::singleShot(0, this, SLOT(checkConnection()));
}

void Instance::checkConnection()
{
	if( mSocket->waitForConnected() )
	{
		mServer = 0;
		mSocket->write("inst\n");
		mSocket->waitForBytesWritten();
		qApp->quit();
	}
	else
	{
		delete mSocket;
		mSocket = 0;

		mServer = new QLocalServer;
		mServer->listen("frosty_instance");

		connect(mServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

		startup();
	}
}

void Instance::startup()
{
	if( settings->email().isEmpty() && !settings->offline() )
	{
		(new Register)->show();
	}
	else
	{
		mLoginOK = true;
		Taskbar::getSingletonPtr()->show();
	}

	if( !qApp->arguments().contains("--no-check") )
	{
		VersionCheck::getSingletonPtr();

		(new IconImport)->performCheck();
	}
}

Instance::~Instance()
{
	foreach(QLocalSocket *client, mClients)
		delete client;

	delete mSocket;
	delete mServer;
}

Instance* Instance::getSingletonPtr()
{
	Q_ASSERT(g_instance_inst);

	return g_instance_inst;
}

void Instance::newConnection()
{
	QLocalSocket *client = mServer->nextPendingConnection();
	if(!client)
		return;

	connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));

	mClients << client;

	client->open(QIODevice::ReadWrite);
}

void Instance::disconnected()
{
	QLocalSocket *client = qobject_cast<QLocalSocket*>(sender());
	if(!client)
		return;

	mClients.removeAt( mClients.indexOf(client) );

	delete client;
}

void Instance::readyRead()
{
	QLocalSocket *client = qobject_cast<QLocalSocket*>(sender());
	if(!client)
		return;

	while( client->canReadLine() )
	{
		QString line = client->readLine();
		if(line.trimmed() == "inst" && mLoginOK)
		{
			Taskbar::getSingletonPtr()->show();
			Taskbar::getSingletonPtr()->activateWindow();
			Taskbar::getSingletonPtr()->raise();
		}
	}
}

void Instance::loginOK()
{
	mLoginOK = true;
}
