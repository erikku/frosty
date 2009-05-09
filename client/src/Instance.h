/******************************************************************************\
*  client/src/Instance.h                                                       *
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

#ifndef __Instance_h__
#define __Instance_h__

#include <QtCore/QObject>
#include <QtCore/QList>

class QLocalSocket;
class QLocalServer;

class Instance : public QObject
{
	Q_OBJECT

public:
	Instance(QObject *parent = 0);
	~Instance();

	static Instance* getSingletonPtr();

public slots:
	void loginOK();

protected slots:
	void checkConnection();
	void newConnection();
	void disconnected();
	void readyRead();

protected:
	void startup();

	QLocalSocket *mSocket;
	QLocalServer *mServer;

	QList<QLocalSocket*> mClients;

	bool mLoginOK;
};

#endif // __Instance_h__
