/******************************************************************************\
*  updater/src/CheckThread.h                                                   *
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

#ifndef __CheckThread_h__
#define __CheckThread_h__

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QThread>

typedef QMap<QString,QString> StringMap;

class CheckThread : public QThread
{
	Q_OBJECT

public:
	CheckThread(QObject *parent = 0);

	virtual void run();

	void setChecksums(const QString& checksums);

signals:
	void listReady(const StringMap& list);

protected:
	QString fileChecksum(const QString& path) const;
	bool checkFile(const QString& path, const QString& checksum) const;

	QString mChecksums;
};

#endif // __CheckThread_h__
