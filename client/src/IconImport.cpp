/******************************************************************************\
*  client/src/IconImport.cpp                                                   *
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

#include "IconImport.h"

#include <QtCore/QDir>
#include <QtCore/QTimer>

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

IconImport::IconImport(QWidget *parent_widget) : QWidget(parent_widget)
{
	mProc = new QProcess(this);
	mSrcPath = "C:\\AeriaGames\\MegaTen";

#ifdef Q_OS_WIN32
	mAppPath = QDir::toNativeSeparators(qApp->applicationDirPath() +
		"/convert.exe");
#else
	mAppPath = "/usr/bin/convert";
#endif // Q_OS_WIN32

	connect(mProc, SIGNAL(finished(int, QProcess::ExitStatus)),
		this, SLOT(iconFinished(int, QProcess::ExitStatus)));
	connect(mProc, SIGNAL(error(QProcess::ProcessError)),
		this, SLOT(iconError()));

	ui.setupUi(this);

	setWindowTitle( tr("%1 - Icon Import").arg(
		tr("Shin Megami Tensei IMAGINE DB") ) );
}

void IconImport::performCheck()
{
	QDir appDir( qApp->applicationDirPath() );
	appDir.mkpath("icons/devils");
	appDir.mkpath("icons/skills");
	appDir.mkpath("icons/mashou");
	appDir.mkpath("icons/items");

	QFileInfo check("icons/items/icon_A09_0100a.png");
	if( check.exists() )
	{
		deleteLater();

		return;
	}

	// Ask if we should import
	{
		QMessageBox box(QMessageBox::Critical, tr("Icon Import"),
			tr("One or more icons need to be imported from the game to "
			"be used in database. If you choose to import the icons, you "
			"will be prompted for the location of your game data (the "
			"directory that contains the <i>ImagineClient.exe</i> file."
			"\n\nWould you like to import them now?"),
			QMessageBox::Yes | QMessageBox::No);

		if(box.exec() == QMessageBox::No)
		{
			deleteLater();

			return;
		}
	}

	QString installLocation = QFileDialog::getExistingDirectory(0, tr("Game "
		"Data Location"), mSrcPath);
	if( !installLocation.isEmpty() )
		mSrcPath = installLocation;

	show();

	QTimer::singleShot(1, this, SLOT(scanIcons()));
}

void IconImport::scanIcons()
{
	QDir dir(mSrcPath);
	if( !dir.cd("Interface") )
	{
		QMessageBox box(QMessageBox::Critical, tr("Icon Import Errpr"),
			tr("Failed to find the <b>Interface</b> directory."));

		box.exec();
		deleteLater();

		return;
	}

	if( !dir.cd("tga") )
	{
		QMessageBox box(QMessageBox::Critical, tr("Icon Import Error"),
			tr("Failed to find the <b>Interface/tga</b> directory."));

		box.exec();
		deleteLater();

		return;
	}

	mDevils = dir.entryList(QStringList()
		<< "icon_D[1-3]_*.[Tt][Gg][Aa]", QDir::Files);

	mMashou = dir.entryList(QStringList()
		<< "icon_I71_*.[Tt][Gg][Aa]", QDir::Files);

	mSkills = dir.entryList(QStringList() << "icon_SB[0-9][0-9]_*.[Tt][Gg][Aa]"
		<< "icon_SD[0-9][0-9]_*.[Tt][Gg][Aa]"
		<< "icon_SM[0-9][0-9]_*.[Tt][Gg][Aa]"
		<< "icon_SP[0-9][0-9]_*.[Tt][Gg][Aa]"
		<< "icon_SS[0-9][0-9]_*.[Tt][Gg][Aa]", QDir::Files);

	mItems = dir.entryList(QStringList() << "icon_A[0-9][0-9]_*.[Tt][Gg][Aa]"
		<< "icon_I[0-9][0-9]_*.[Tt][Gg][Aa]"
		<< "icon_W[0-9][0-9]_*.[Tt][Gg][Aa]", QDir::Files);

	ui.iconStatus->setMaximum( mDevils.count() + mSkills.count() +
		mMashou.count() + mItems.count() );

	QTimer::singleShot(1, this, SLOT(handleNext()));
}

void IconImport::handleNext()
{
	QString src, dest;
	if( !mDevils.isEmpty() )
	{
		dest = "devils/";
		src = mDevils.takeFirst();
	}
	else if( !mSkills.isEmpty() )
	{
		dest = "skills/";
		src = mSkills.takeFirst();
	}
	else if( !mMashou.isEmpty() )
	{
		dest = "mashou/";
		src = mMashou.takeFirst();
	}
	else if( !mItems.isEmpty() )
	{
		dest = "items/";
		src = mItems.takeFirst();
	}
	else
	{
		deleteLater();
		return;
	}

	dest += src.left(src.length() - 4) + ".png";
	mCurrent = dest;

	ui.iconLabel->setText(dest);

	dest = QDir::toNativeSeparators(qApp->applicationDirPath() +
		QString("/icons/") + dest);

	src = QDir::toNativeSeparators(QString(mSrcPath)
		+ QString("/Interface/tga/") + src);

	mLastSrc = src;
	mLastDest = dest;

	QFileInfo info(dest);
	if( info.exists() )
	{
		iconFinished(0, QProcess::NormalExit);

		ui.iconStatus->setValue(ui.iconStatus->value() + 1);

		return;
	}

	QStringList args;
	args << src << dest;

	mProc->start(mAppPath, args);
}

void IconImport::iconFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitStatus);

	if(exitCode != 0)
	{
		QMessageBox box(QMessageBox::Critical, tr("Icon Import Error"),
			tr("Failed to import icon: %1").arg(mCurrent));

		box.exec();
		deleteLater();

		return;
	}

	if( !mLastSrc.isEmpty() )
	{
		QString src = mLastSrc;
		QString dest = mLastDest.replace("icon_", "icon16_");
		mCurrent = QFileInfo(dest).fileName();

		mLastSrc.clear();
		mLastDest.clear();

		QFileInfo info(dest);
		if( info.exists() )
		{
			QTimer::singleShot(1, this, SLOT(handleNext()));

			return;
		}

		QStringList args;
		args << "-scale" << "16x16" << src << dest;

		mProc->start(mAppPath, args);

		return;
	}

	ui.iconStatus->setValue(ui.iconStatus->value() + 1);

	QTimer::singleShot(1, this, SLOT(handleNext()));
}

void IconImport::iconError()
{
	QMessageBox box(QMessageBox::Critical, tr("Icon Import Error"),
		tr("Failed to import icon: %1").arg(mCurrent));

	box.exec();
	deleteLater();
}
