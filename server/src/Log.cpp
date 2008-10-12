/******************************************************************************\
*  server/src/Log.cpp                                                          *
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

#include "Log.h"
#include "Config.h"

#include <QtCore/QFile>
#include <QtCore/QObject>
#include <iostream>

using namespace std;

static Log *g_log_inst = 0;

Log::Log() : mLogFile(0)
{
	// Code here
};

Log* Log::getSingletonPtr()
{
	if(!g_log_inst)
		g_log_inst = new Log;

	Q_ASSERT(g_log_inst);

	return g_log_inst;
};

void Log::loadConfig()
{
	if(!conf->logPath().isEmpty() && !mLogFile)
	{
		mLogFile = new QFile( conf->logPath() );
		mLogFile->open(QIODevice::WriteOnly | QIODevice::Truncate);
		mLogFile->write( QString("Log Started!\n").toUtf8() );
		mLogFile->flush();
	}
};

void Log::critical(const QString& msg)
{
	if( !conf->logCritical() )
		return;

	QString final = QObject::tr("CRITICAL: %1").arg(msg);

	if(mLogFile)
	{
		mLogFile->write( final.toUtf8() );
		mLogFile->flush();
	}
	else
		cerr << final.toLocal8Bit().data() << endl;
};

void Log::error(const QString& msg)
{
	if( !conf->logError() )
		return;

	QString final = QObject::tr("ERROR: %1").arg(msg);

	if(mLogFile)
	{
		mLogFile->write( final.toUtf8() );
		mLogFile->flush();
	}
	else
		cerr << final.toLocal8Bit().data() << endl;
};

void Log::warning(const QString& msg)
{
	if( !conf->logWarning() )
		return;

	QString final = QObject::tr("WARNING: %1").arg(msg);

	if(mLogFile)
	{
		mLogFile->write( final.toUtf8() );
		mLogFile->flush();
	}
	else
		cerr << final.toLocal8Bit().data() << endl;
};

void Log::info(const QString& msg)
{
	if( !conf->logInfo() )
		return;

	QString final = msg;

	if(mLogFile)
	{
		mLogFile->write( final.toUtf8() );
		mLogFile->flush();
	}
	else
		cout << final.toLocal8Bit().data() << endl;
};

void Log::debug(const QString& msg)
{
	if( !conf->logDebug() )
		return;

	QString final = QObject::tr("Debug: %1").arg(msg);

	if(mLogFile)
	{
		mLogFile->write( final.toUtf8() );
		mLogFile->flush();
	}
	else
		cout << final.toLocal8Bit().data() << endl;
};
