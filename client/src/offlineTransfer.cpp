/******************************************************************************\
*  client/src/offlineTransfer.cpp                                              *
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

#include "offlineTransfer.h"

#include <Backend.h>
#include <Config.h>
#include <Auth.h>
#include <Log.h>

#include "sha1.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtGui/QInputDialog>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

static offlineTransfer *g_offline_transfer_inst = 0;

offlineTransfer::offlineTransfer(QObject *parent) : baseTransfer(parent)
{
	qsrand( QDateTime::currentDateTime().toTime_t() );

	/// Load the server config
	conf->loadConfig(":/config.xml");

	if(conf->dbType() == "sqlite")
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "master");
		db.setDatabaseName( conf->dbPath() );
		if( !db.open() )
			LOG_ERROR( db.lastError().text() );
	}
	else // mysql
	{
		db = QSqlDatabase::addDatabase("QMYSQL", "master");
		db.setHostName( conf->dbHost() );
		db.setDatabaseName( conf->dbName() );
		db.setUserName( conf->dbUser() );
		db.setPassword( conf->dbPass() );
		db.open();

		QSqlQuery query("SET CHARSET utf8", db);
		query.exec();
	}

	if(conf->userDBType() == "sqlite")
	{
		bool create = !QFileInfo( conf->userDBPath() ).exists();

		user_db = QSqlDatabase::addDatabase("QSQLITE", "user");
		user_db.setDatabaseName( conf->userDBPath() );
		if( !user_db.open() )
			LOG_ERROR( user_db.lastError().text() );

		if(create)
		{
			QFile sql(":/user.sql");
			sql.open(QIODevice::ReadOnly);

			QStringList lines = QString::fromUtf8( sql.readAll() ).split("\n");
			foreach(QString line, lines)
			{
				QRegExp lineMatcher("^(.+);$");
				if( line.isEmpty() || !lineMatcher.exactMatch(line) )
					continue;

				QSqlQuery query(user_db);
				query.exec( lineMatcher.cap(1) );
			}

			sql.close();
		}
	}
	else // mysql
	{
		user_db = QSqlDatabase::addDatabase("QMYSQL", "user");
		user_db.setHostName( conf->userDBHost() );
		user_db.setDatabaseName( conf->userDBName() );
		user_db.setUserName( conf->userDBUser() );
		user_db.setPassword( conf->userDBPass() );
		user_db.open();

		QSqlQuery query("SET CHARSET utf8", user_db);
		query.exec();
	}

	mBackend = new Backend;

	{
		QString hash, path = QDir::homePath();
		unsigned char checksum[20];

		sha1((uchar*)path.toUtf8().data(), hash.toUtf8().size(), checksum);

		for(int i = 0; i < 20; i++)
		{
			int byte = (int)checksum[i];
			hash += QString("%1").arg(byte, 2, 16, QLatin1Char('0'));
		}

		mEmail = QString("%1@local.server").arg(hash);

		if(auth->queryUserID(mEmail) <= 0)
		{
			QString username = QInputDialog::getText(0,
				tr("Display Name Entry"),
				tr("Please enter a display name for yourself."));
			if( username.isEmpty() )
				username = hash;

			auth->registerUser(mEmail, username, "blank_for_a_reason");
		}
	}
}

offlineTransfer::~offlineTransfer()
{
	delete mBackend;
	delete auth;

	db.close();
	db = QSqlDatabase();

	user_db.close();
	user_db = QSqlDatabase();
}

offlineTransfer* offlineTransfer::inst()
{
	if(!g_offline_transfer_inst)
		g_offline_transfer_inst = new offlineTransfer;

	return g_offline_transfer_inst;
}

offlineTransfer* offlineTransfer::start(const QVariant& req)
{
	return inst()->startReal(req);
}

offlineTransfer* offlineTransfer::startReal(const QVariant& req)
{
	QVariantList set = mBackend->parseRequestInternal(
		0, db, user_db, req, mEmail);
	QListIterator<QVariant> it(set);
	while( it.hasNext() )
	{
		QVariantMap resp = it.next().toMap();

		emit transferFinished(resp, resp.value("user_data").toString());
	}

	return this;
}
