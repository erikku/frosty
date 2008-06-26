/******************************************************************************\
*  updater/src/Updater.h                                                       *
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

#ifndef __Updater_h__
#define __Updater_h__

#include <QtCore/QMap>
#include <QtGui/QWidget>
#include <QtNetwork/QHttpResponseHeader>

class Progress;

class Updater : public QWidget
{
	Q_OBJECT

public:
	Updater(QWidget *parent = 0);
	~Updater();

	QString fileChecksum(const QString& path) const;
	QMap<QString, QString> checkFiles(const QString& checksums) const;
	bool checkFile(const QString& path, const QString& checksum) const;
	void downloadFile(const QString& path);

protected slots:
	void transferFailed();
	void transferFinished(const QString& checksum);

protected:
	int mCount;
	Progress *mProgress;

	QString mChecksums;
	QString mCurrentPath;
	QMap<QString, QString> mBadList;
};

#endif // __Updater_h__
