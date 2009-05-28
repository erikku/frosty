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
#include "CheckThread.h"
#include "HttpTransfer.h"

#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtGui/QApplication>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include <QtNetwork/QHttp>

#ifdef Q_OS_WIN32
#define UPDATE_FILE "updates.win.sha1"
#define UPDATE_EXE  "frosty_updater.exe"
#define EXE_FILE "frosty.exe"
#else
#define UPDATE_FILE "updates.lin.sha1"
#define UPDATE_EXE  "frosty_updater"
#define EXE_FILE "frosty"
#endif

Q_DECLARE_METATYPE(StringMap)

Updater::Updater(QWidget *parent_widget) : QWidget(parent_widget), mCount(0)
{
	qRegisterMetaType<StringMap>();

	QCoreApplication::setOrganizationName("Absolutely Frosty Team");
	QCoreApplication::setOrganizationDomain("troopersklan.jp");
	QCoreApplication::setApplicationName("Absolutely Frosty");

	ui.setupUi(this);
	setWindowTitle( tr("Absolutely Frosty Updater") );

	// http://www.troopersklan.jp/megaten/updates
	QUrl url("http://gigadelic.homelinux.net:10900/frosty/updates");

	QSettings settings;
	if( !settings.contains("update_url") )
	{
		QString text = QInputDialog::getText(0, tr("Absolutely Frosty Update "
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
}

void Updater::checkFiles(const QString& checksums)
{
	ui.fileLabel->setText( tr("Checking files, please wait...") );

	CheckThread *checkThread = new CheckThread;

	connect(checkThread, SIGNAL(listReady(const StringMap&)),
		this, SLOT(listReady(const StringMap&)));
	connect(checkThread, SIGNAL(finished()),
		checkThread, SLOT(deleteLater()));

	checkThread->setChecksums(checksums);
	checkThread->start();
}

void Updater::listReady(const StringMap& list)
{
	mBadList = list;

	if( mBadList.contains(UPDATE_EXE) )
		mBadList.remove(UPDATE_EXE);

	ui.totalProgress->setMaximum(mBadList.count());
	ui.totalProgress->setValue(0);

	mCount--;
	transferFinished(QString());
}

void Updater::downloadFile(const QString& path)
{
	ui.fileLabel->setText(path);

	QSettings settings;
	QUrl url = settings.value("update_url").toUrl();

	url = QString("%1/%2").arg(url.toString()).arg(path);

	HttpTransfer *transfer = HttpTransfer::start(url, path,
		QStringMap(), true);

	connect(transfer, SIGNAL(transferFinished(const QString&)),
		this, SLOT(transferFinished(const QString&)));
	connect(transfer, SIGNAL(progressChanged(int)),
		ui.fileProgress, SLOT(setValue(int)));
	connect(transfer, SIGNAL(transferFailed()),
		this, SLOT(transferFailed()));
}

void Updater::transferFailed()
{
	if( mChecksums.isEmpty() )
	{
		QMessageBox::critical(0, tr("Absolutely Frosty Updater"),
			tr("Failed to download update list"));
	}
	else
	{
		QMessageBox::critical(0, tr("Absolutely Frosty Updater"),
			tr("Failed to download file"));
	}

	qApp->quit();
}

void Updater::transferFinished(const QString& checksum)
{
	HttpTransfer *transfer = qobject_cast<HttpTransfer*>(sender());
	if(transfer)
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
		checkFiles(mChecksums);

		return;
	}
	else if( !mCurrentPath.isEmpty() )
	{
		if(mBadList.value(mCurrentPath) != checksum)
		{
			QMessageBox::critical(0, tr("Absolutely Frosty Updater"),
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

		ui.totalProgress->setMaximum(10);
		ui.totalProgress->setValue(10);

		ui.fileLabel->setText( tr("Update Finished") );
		ui.startButton->setEnabled(true);
	}
}

void Updater::startApp()
{
	QProcess::startDetached(QDir::current().filePath(EXE_FILE), QStringList());
	qApp->quit();
}
