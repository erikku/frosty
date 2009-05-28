/******************************************************************************\
*  client/src/VersionCheck.cpp                                                 *
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

#include "VersionCheck.h"
#include "HttpTransfer.h"
#include "Settings.h"
#include "ajax.h"
#include "json.h"
#include "sha1.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

static VersionCheck *g_versioncheck_inst = 0;

VersionCheck::VersionCheck(QObject *parent_object) : QObject(parent_object),
	mHaveVersionFile(false)
{
	ajax::getSingletonPtr()->subscribe(this);

#ifdef CHECK_SERVER_VERSION
	{
		QVariantMap action;
		action["action"] = "server_updates";
		action["user_data"] = "server_updates";

		ajax::getSingletonPtr()->request(action);
	}
#else // CHECK_SERVER_VERSION
	QString updateUrl = settings->updateUrl().toString();

	HttpTransfer *transfer = HttpTransfer::start(
		QUrl(updateUrl + "/version.json"),
		QDir(qApp->applicationDirPath()).filePath("version.json") );

	connect(transfer, SIGNAL(transferFailed()),
		this, SLOT(transferFailed()));
	connect(transfer, SIGNAL(transferFinished(const QString&)),
		this, SLOT(transferFinished(const QString&)));
#endif // CHECK_SERVER_VERSION
}

VersionCheck* VersionCheck::getSingletonPtr()
{
	if(!g_versioncheck_inst)
		g_versioncheck_inst = new VersionCheck;

	Q_ASSERT(g_versioncheck_inst);

	return g_versioncheck_inst;
}

void VersionCheck::ajaxResponse(const QVariantMap& resp,
	const QString& user_data)
{
	if(user_data == "server_updates")
	{
#ifdef Q_OS_WIN32
		QString client_hash = resp.value("client_win32").toString();
		QString updater_hash = resp.value("updater_win32").toString();
#else
		QString client_hash = resp.value("client_linux").toString();
		QString updater_hash = resp.value("updater_linux").toString();
#endif

		QString client = qApp->applicationFilePath();
		QString updater = QDir( qApp->applicationDirPath() ).filePath(
#ifdef Q_OS_WIN32
			"frosty_updater.exe");
#else
			"frosty_updater");
#endif

		mUpdaterPath = updater;
		mUpdaterHash = updater_hash;

		QRegExp hashMatcher("[0-9a-f]{40}");

		if(hashMatcher.exactMatch(updater_hash) &&
			sha1HashFile(updater) != updater_hash)
		{
			QMessageBox::StandardButton button = QMessageBox::warning(0,
				tr("Updater Needs Updating"), tr("A new version of updater is "
				"avaliable. It is recommended you this update. Would you like "
				"to download it now?"),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

			QString updateUrl = settings->updateUrl().toString();
			if(button == QMessageBox::Yes)
			{
				HttpTransfer *transfer = HttpTransfer::start(
#ifdef Q_OS_WIN32
					QUrl(updateUrl + "/frosty_updater.exe"),
#else
					QUrl(updateUrl + "/frosty_updater"),
#endif
					updater, QStringMap(), true);

				connect(transfer, SIGNAL(transferFailed()),
					this, SLOT(transferFailed()));
				connect(transfer, SIGNAL(transferFinished(const QString&)),
					this, SLOT(transferFinished(const QString&)));

				return;
			}
		}

		if(hashMatcher.exactMatch(client_hash) &&
			sha1HashFile(client) != client_hash)
		{
			QMessageBox::warning(0, tr("Client Needs Updating"), tr("A new "
				"version of the client is avaliable for downloading. It is "
				"recommended you exit the client now and run the updater."));
		}
	}
}

QString VersionCheck::sha1HashFile(const QString& path) const
{
	sha1_context ctx;
	QByteArray buffer;
	unsigned char checksum[20];

	sha1_starts(&ctx);

	QFile file(path);
	if( !file.open(QIODevice::ReadOnly) )
		return QString();

	while( !(buffer = file.read(4096)).isEmpty() )
		sha1_update(&ctx, (unsigned char*)buffer.data(), buffer.size());

	sha1_finish(&ctx, checksum);

	QString hash;
	for(int i = 0; i < 20; i++)
		hash += QString("%1").arg(((int)checksum[i]), 2, 16, QLatin1Char('0'));

	return hash;
}

void VersionCheck::transferFailed()
{
	QMessageBox::critical(0, tr("Updater Download Failed"),
		tr("The updater failed to download from the update server. Please make "
		"sure your settings are correct and try again."));
}

void VersionCheck::transferFinished(const QString& checksum)
{
#ifndef CHECK_SERVER_VERSION
	if(!mHaveVersionFile)
	{
		QFile file( QDir(qApp->applicationDirPath()).filePath("version.json") );
		if( !file.open(QIODevice::ReadOnly) )
			return;

		QString line = file.readLine().trimmed();
		QVariantMap versions = json::parse(line).toMap();

		ajaxResponse(versions, "server_updates");

		file.close();
		file.remove();

		mHaveVersionFile = true;

		return;
	}
#endif // CHECK_SERVER_VERSION

	QFile file(mUpdaterPath);
	file.setPermissions(QFile::ReadOwner | QFile::WriteOwner |
		QFile::ExeOwner | QFile::ReadGroup | QFile::ExeGroup |
		QFile::ReadOther | QFile::ExeOther);

	if(mUpdaterHash != checksum)
	{
		QMessageBox::critical(0, tr("Checksum Mismatch"), tr("The updater "
			"checksum does not match the one provided by the server."));
	}
	else
	{
		QMessageBox::StandardButton button = QMessageBox::information(0,
			tr("Updater Downloaded"), tr("The new updater has been downloaded. "
			"It is recommended you quit the client now and run the updater. "
			"Would you like to do this now?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if(button == QMessageBox::Yes)
		{
			QProcess::startDetached(mUpdaterPath);
			qApp->quit();
		}
	}
}
