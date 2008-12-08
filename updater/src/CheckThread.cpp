/******************************************************************************\
*  updater/src/CheckThread.cpp                                                 *
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

#include "CheckThread.h"
#include "sha1.h"

#include <QtCore/QRegExp>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

CheckThread::CheckThread(QObject *parent_object) : QThread(parent_object)
{
	// Nothing to see here
}

void CheckThread::run()
{
	QString stream_text = mChecksums;
	QTextStream stream(&stream_text);

	QRegExp checksumMatcher("([0-9A-Fa-f]{40}) \\*(.+)");

	QString line;
	StringMap badList;

	while(true)
	{
		line = stream.readLine();
		if( line.isEmpty() )
			break;

		if( !checksumMatcher.exactMatch(line) )
			continue;

		if( !checkFile(checksumMatcher.cap(2), checksumMatcher.cap(1)) )
			badList[checksumMatcher.cap(2)] = checksumMatcher.cap(1);
	}

	emit listReady(badList);

	exit();
}

void CheckThread::setChecksums(const QString& checksums)
{
	mChecksums = checksums;
}

QString CheckThread::fileChecksum(const QString& path) const
{
	// TODO: Add error checking

	sha1_context ctx;
	sha1_starts(&ctx);

	qint64 bytesRead = -1;
	unsigned char buffer[4096];
	unsigned char hash[20];

	QFile file(path);
	file.open(QIODevice::ReadOnly);

	while( (bytesRead = file.read((char*)buffer, 4096)) > 0 )
		sha1_update(&ctx, buffer, (int)bytesRead);

	sha1_finish(&ctx, hash);
	file.close();

	QString checksum;
	for(int i = 0; i < 20; i++)
	{
		int byte = (int)hash[i];
		checksum += QString("%1").arg(byte, 2, 16, QLatin1Char('0'));
	}

	//checksum += QString(" *%1").arg(path);

	return checksum;
}

bool CheckThread::checkFile(const QString& path, const QString& checksum) const
{
	if( !QFileInfo(path).exists() )
		return false;

	return ( fileChecksum(path).toLower() == checksum.toLower() );
}
