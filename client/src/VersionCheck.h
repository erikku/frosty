/******************************************************************************\
*  client/src/VersionCheck.h                                                   *
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

#ifndef __VersionCheck_h__
#define __VersionCheck_h__

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

class VersionCheck : public QObject
{
	Q_OBJECT

public:
	static VersionCheck* getSingletonPtr();

protected slots:
	void transferFailed();
	void transferFinished(const QString& checksum);

	void ajaxResponse(const QVariant& resp);

protected:
	VersionCheck(QObject *parent = 0);

	QString sha1HashFile(const QString& path) const;

	QString mUpdaterPath;
	QString mUpdaterHash;
};

#endif // __VersionCheck_h__
