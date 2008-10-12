/******************************************************************************\
*  updater/src/Updater.cpp                                                     *
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

#include "Updater.h"
#include "HttpTransfer.h"
#include "sha1.h"

#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QSettings>
#include <QtCore/QRegExp>
#include <QtGui/QApplication>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include <QtNetwork/QHttp>

#ifdef Q_OS_WIN32
#define UPDATE_FILE "updates.win.sha1"
#define EXE_FILE "megatendb.exe"
#else
#define UPDATE_FILE "updates.lin.sha1"
#define EXE_FILE "megatendb"
#endif

Updater::Updater(QWidget *parent) : QWidget(parent), mCount(0)
{
	QCoreApplication::setOrganizationName("MegatenDB");
	QCoreApplication::setOrganizationDomain("troopersklan.jp");
	QCoreApplication::setApplicationName("Megaten DB");

	ui.setupUi(this);
	setWindowTitle( tr("Megaten DB Updater") );

	// http://www.troopersklan.jp/megaten/updates
	QUrl url("http://gigadelic.homelinux.net:10900/megatendb/updates");

	QSettings settings;
	if( !settings.contains("update_url") )
	{
		QString text = QInputDialog::getText(0, tr("Megaten DB Update "
			"Location"), tr("URL:"), QLineEdit::Normal, url.toString());

		if( !text.isEmpty() )
			url = text;

		settings.setValue("update_url", url);
	}

	url = settings.value("update_url").toUrl();

	ui.logView->load( QUrl(url.toString() + "/index.php") );

	connect(ui.startButton, SIGNAL(clicked(bool)), this, SLOT(startApp()));

	downloadFile(UPDATE_FILE);
	show();
};

QString Updater::fileChecksum(const QString& path) const
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
};

bool Updater::checkFile(const QString& path, const QString& checksum) const
{
	if( !QFileInfo(path).exists() )
		return false;

	return ( fileChecksum(path).toLower() == checksum.toLower() );
};

QMap<QString, QString> Updater::checkFiles(const QString& checksums) const
{
	QString stream_text = checksums;
	QTextStream stream(&stream_text);

	QRegExp checksumMatcher("([0-9A-Fa-f]{40}) \\*(.+)");

	QString line;
	QMap<QString, QString> badList;

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

	return badList;
};

void Updater::downloadFile(const QString& path)
{
	ui.fileLabel->setText(path);

	QSettings settings;
	QUrl url = settings.value("update_url").toUrl();

	url = QString("%1/%2").arg(url.toString()).arg(path);

	HttpTransfer *transfer = HttpTransfer::start(url, path);

	connect(transfer, SIGNAL(transferFinished(const QString&)),
		this, SLOT(transferFinished(const QString&)));
	connect(transfer, SIGNAL(progressChanged(int)),
		ui.fileProgress, SLOT(setValue(int)));
	connect(transfer, SIGNAL(transferFailed()),
		this, SLOT(transferFailed()));
};

void Updater::transferFailed()
{
	if( mChecksums.isEmpty() )
	{
		QMessageBox::critical(0, tr("Megaten DB Updater"),
			tr("Failed to download update list"));
	}
	else
	{
		QMessageBox::critical(0, tr("Megaten DB Updater"),
			tr("Failed to download file"));
	}

	qApp->quit();
};

void Updater::transferFinished(const QString& checksum)
{
	HttpTransfer *transfer = qobject_cast<HttpTransfer*>(sender());
	if(!transfer)
		return;

	transfer->deleteLater();

	mCount++;
	ui.totalProgress->setValue(mCount);

	if( mChecksums.isEmpty() )
	{
		QFile file(UPDATE_FILE);
		file.open(QIODevice::ReadOnly);

		mChecksums = QString::fromUtf8( file.readAll() );

		file.close();
		file.remove();

		mCount = 0;
		mBadList = checkFiles(mChecksums);
		ui.totalProgress->setValue(0);
		ui.totalProgress->setMaximum(mBadList.count());
	}
	else
	{
		if(mBadList.value(mCurrentPath) != checksum)
		{
			QMessageBox::critical(0, tr("Megaten DB Updater"),
				tr("Failed to download file: Wrong checksum"));

			qApp->quit();
			return;
		}

		mBadList.remove(mCurrentPath);
	}

	if( !mBadList.keys().isEmpty() )
	{
		mCurrentPath = mBadList.keys().first();
		downloadFile(mCurrentPath);
	}
	else
	{
		QFile file( QDir::current().filePath(EXE_FILE) );
		file.setPermissions(QFile::ReadOwner | QFile::WriteOwner |
			QFile::ExeOwner | QFile::ReadGroup | QFile::ExeGroup |
			QFile::ReadOther | QFile::ExeOther);

		ui.fileLabel->setText( tr("Update Finished") );
		ui.startButton->setEnabled(true);
	}
};

void Updater::startApp()
{
	QProcess::startDetached( QDir::current().filePath(EXE_FILE) );
	qApp->quit();
};
