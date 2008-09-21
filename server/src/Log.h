/******************************************************************************\
*  server/src/Log.h                                                            *
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

#ifndef __Log_h__
#define __Log_h__

#include <QtCore/QString>

class QFile;

class Log
{
public:
	static Log* getSingletonPtr();

	void loadConfig();

	void critical(const QString& msg);
	void error(const QString& msg);
	void warning(const QString& msg);
	void info(const QString& msg);
	void debug(const QString& msg);

protected:
	Log();

	QFile *mLogFile;
};

#define LOG_CRITICAL(msg) Log::getSingletonPtr()->critical(msg)
#define LOG_ERROR(msg)    Log::getSingletonPtr()->error(msg)
#define LOG_WARNING(msg)  Log::getSingletonPtr()->warning(msg)
#define LOG_INFO(msg)     Log::getSingletonPtr()->info(msg)
#define LOG_DEBUG(msg)    Log::getSingletonPtr()->debug(msg)

#endif // __Log_h__
