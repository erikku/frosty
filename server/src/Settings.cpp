/******************************************************************************\
*  server/src/Settings.cpp                                                     *
*  Copyright (C) 2009 John Eric Martin <john.eric.martin@gmail.com>            *
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

#include "Settings.h"
#include "Config.h"

#include <QtCore/QDir>

#include <QtGui/QFileDialog>
#include <QtGui/QApplication>

#ifndef QT_NO_DEBUG
#include <iostream>
#endif // QT_NO_DEBUG

Settings::Settings(QWidget *parent_widget) : QWidget(parent_widget)
{
	ui.setupUi(this);

	setWindowTitle( tr("%1 - Server Configuration").arg(
		tr("Absolutely Frosty") ) );

	darkenWidget(ui.connectionLabel);
	darkenWidget(ui.databaseLabel);
	darkenWidget(ui.logLabel);
	darkenWidget(ui.shoutboxLabel);
	darkenWidget(ui.sslLabel);
	darkenWidget(ui.authDatabaseLabel);
	darkenWidget(ui.permsLabel);
	darkenWidget(ui.adminLabel);
	darkenWidget(ui.saltsLabel);
	darkenWidget(ui.lettersLabel);
	darkenWidget(ui.clientLabel);
	darkenWidget(ui.updaterLabel);

	QIcon browse(":/fileopen.png");
	ui.databaseBrowse->setIcon(browse);
	ui.logBrowse->setIcon(browse);
	ui.shoutboxBrowse->setIcon(browse);
	ui.sslCertBrowse->setIcon(browse);
	ui.sslKeyBrowse->setIcon(browse);
	ui.authBrowse->setIcon(browse);
	ui.fontBrowse->setIcon(browse);

	connect(ui.connectionType, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateConnectionType()));
	connect(ui.databaseBrowse, SIGNAL(clicked(bool)),
		this, SLOT(browseDatabasePath()));
	connect(ui.logBrowse, SIGNAL(clicked(bool)),
		this, SLOT(browseLogPath()));
	connect(ui.shoutboxBrowse, SIGNAL(clicked(bool)),
		this, SLOT(browseShoutboxLogPath()));
	connect(ui.sslCertBrowse, SIGNAL(clicked(bool)),
		this, SLOT(browseCertPath()));
	connect(ui.sslKeyBrowse, SIGNAL(clicked(bool)),
		this, SLOT(browseKeyPath()));
	connect(ui.authBrowse, SIGNAL(clicked(bool)),
		this, SLOT(browseAuthDatabasePath()));
	connect(ui.fontBrowse, SIGNAL(clicked(bool)),
		this, SLOT(browseFontPath()));

	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(save()));

	setAttribute(Qt::WA_DeleteOnClose);
}

void Settings::load()
{
	QString addr = conf->address();
	if(addr == "any")
	{
		ui.connectionIP->clear();
		ui.connectionType->setCurrentIndex(0);
	}
	else if(addr == "localhost")
	{
		ui.connectionIP->clear();
		ui.connectionType->setCurrentIndex(1);
	}
	else
	{
		ui.connectionIP->setText(addr);
		ui.connectionType->setCurrentIndex(2);
	}

	ui.connectionPort->setValue( conf->port() );

	ui.databasePath->setText( conf->dbPath() );
	ui.logPath->setText( conf->logPath() );

	ui.logDebug->setCheckState(conf->logDebug() ? Qt::Checked : Qt::Unchecked);
	ui.logInfo->setCheckState(conf->logInfo() ? Qt::Checked : Qt::Unchecked);
	ui.logWarning->setCheckState(conf->logWarning() ?
		Qt::Checked : Qt::Unchecked);
	ui.logError->setCheckState(conf->logError() ? Qt::Checked : Qt::Unchecked);
	ui.logCritical->setCheckState(conf->logCritical() ?
		Qt::Checked : Qt::Unchecked);

	ui.shoutboxLog->setCheckState(conf->shoutboxLog() ?
		Qt::Checked : Qt::Unchecked);
	ui.shoutboxPath->setText( conf->shoutboxLogPath() );

	ui.sig->setText( conf->signature() );

	ui.sslEnabled->setCheckState(conf->sslEnabled() ?
		Qt::Checked : Qt::Unchecked);
	ui.sslCertPath->setText( conf->sslCertPath() );
	ui.sslKeyPath->setText( conf->sslKeyPath() );

	ui.authPath->setText( conf->authDBPath() );

	ui.adminPerm->setCheckState(conf->authAdmin() ?
		Qt::Checked : Qt::Unchecked);
	ui.view_dbPerm->setCheckState(conf->authViewDB() ?
		Qt::Checked : Qt::Unchecked);
	ui.modify_dbPerm->setCheckState(conf->authModifyDB() ?
		Qt::Checked : Qt::Unchecked);
	ui.admin_dbPerm->setCheckState(conf->authAdminDB() ?
		Qt::Checked : Qt::Unchecked);

	ui.adminUser->setText( conf->authAdminUser() );

	ui.imageSalt->setText( conf->saltImg() );
	ui.passwordSalt->setText( conf->saltPass() );

	ui.fontPath->setText( conf->captchaFont() );
	ui.letters->setPlainText( conf->captchaLetters().join(",") );

	ui.windowsClient->setText( conf->clientWin32() );
	ui.macClient->setText( conf->clientMacOSX() );
	ui.linuxClient->setText( conf->clientLinux() );

	ui.windowsUpdater->setText( conf->updaterWin32() );
	ui.macUpdater->setText( conf->updaterMacOSX() );
	ui.linuxUpdater->setText( conf->updaterLinux() );

	show();
}

void Settings::save()
{
	switch( ui.connectionType->currentIndex() )
	{
		case 0:
			conf->setAddress("any");
			break;
		case 1:
			conf->setAddress("localhost");
			break;
		default:
			conf->setAddress( ui.connectionIP->text() );
			break;
	}

	conf->setPort( ui.connectionPort->value() );

	conf->setDBPath( ui.databasePath->text() );
	conf->setLogPath( ui.logPath->text() );

	conf->setLogDebug(ui.logDebug->checkState() == Qt::Checked);
	conf->setLogInfo(ui.logInfo->checkState() == Qt::Checked);
	conf->setLogWarning(ui.logWarning->checkState() == Qt::Checked);
	conf->setLogError(ui.logError->checkState() == Qt::Checked);
	conf->setLogCritical(ui.logCritical->checkState() == Qt::Checked);

	conf->setShoutboxLog(ui.shoutboxLog->checkState() == Qt::Checked);
	conf->setShoutboxLogPath( ui.shoutboxPath->text() );

	conf->setSignature( ui.sig->text() );

	conf->setSslEnabled(ui.sslEnabled->checkState() == Qt::Checked);
	conf->setSslCertPath( ui.sslCertPath->text() );
	conf->setSslKeyPath( ui.sslKeyPath->text() );

	conf->setAuthDBPath( ui.authPath->text() );

	conf->setAuthAdmin(ui.adminPerm->checkState() == Qt::Checked);
	conf->setAuthViewDB(ui.view_dbPerm->checkState() == Qt::Checked);
	conf->setAuthModifyDB(ui.modify_dbPerm->checkState() == Qt::Checked);
	conf->setAuthAdminDB(ui.admin_dbPerm->checkState() == Qt::Checked);

	conf->setAuthAdminUser( ui.adminUser->text() );

	conf->setSaltImg( ui.imageSalt->text() );
	conf->setSaltPass( ui.passwordSalt->text() );

	conf->setCaptchaFont( ui.fontPath->text() );
	conf->setCaptchaLetters( ui.letters->toPlainText().split(",") );

	conf->setClientWin32( ui.windowsClient->text() );
	conf->setClientMacOSX( ui.macClient->text() );
	conf->setClientLinux( ui.linuxClient->text() );

	conf->setUpdaterWin32( ui.windowsUpdater->text() );
	conf->setUpdaterMacOSX( ui.macUpdater->text() );
	conf->setUpdaterLinux( ui.linuxUpdater->text() );

	conf->saveConfig("config.xml");

	close();
}

void Settings::browseLogPath()
{
	QDir app_dir( qApp->applicationDirPath() );

	QString start_dir;
	if( !ui.logPath->text().isEmpty() )
		start_dir = QFileInfo( ui.logPath->text() ).absolutePath();

	QString path = QFileDialog::getSaveFileName(this, tr("Log Path"),
		start_dir, "Server Log (*.log)");
	if( !path.isEmpty() )
		ui.logPath->setText( app_dir.relativeFilePath(path) );
}

void Settings::browseKeyPath()
{
	QDir app_dir( qApp->applicationDirPath() );

	QString start_dir;
	if( !ui.sslKeyPath->text().isEmpty() )
		start_dir = QFileInfo( ui.sslKeyPath->text() ).absolutePath();

	QString path = QFileDialog::getOpenFileName(this, tr("SSL Key Path"),
		start_dir, "SSL Key (*.key)");
	if( !path.isEmpty() )
		ui.sslKeyPath->setText( app_dir.relativeFilePath(path) );
}

void Settings::browseCertPath()
{
	QDir app_dir( qApp->applicationDirPath() );

	QString start_dir;
	if( !ui.sslCertPath->text().isEmpty() )
		start_dir = QFileInfo( ui.sslCertPath->text() ).absolutePath();

	QString path = QFileDialog::getOpenFileName(this, tr("SSL Cert Path"),
		start_dir, "SSL Certificate (*.crt)");
	if( !path.isEmpty() )
		ui.sslCertPath->setText( app_dir.relativeFilePath(path) );
}

void Settings::browseFontPath()
{
	QString start_dir;
	if( !ui.fontPath->text().isEmpty() )
		start_dir = QFileInfo( ui.fontPath->text() ).absolutePath();

	QString path = QFileDialog::getOpenFileName(this, tr("CAPTCHA Font Path"),
		start_dir, "TrueType Font File (*.ttf)");
	if( !path.isEmpty() )
		ui.fontPath->setText(path);
}

void Settings::browseDatabasePath()
{
	QDir app_dir( qApp->applicationDirPath() );

	QString start_dir;
	if( !ui.databasePath->text().isEmpty() )
		start_dir = QFileInfo( ui.databasePath->text() ).absolutePath();

	QString path = QFileDialog::getOpenFileName(this, tr("Database Path"),
		start_dir, "SQLite3 Database (*.db)");
	if( !path.isEmpty() )
		ui.databasePath->setText( app_dir.relativeFilePath(path) );
}

void Settings::browseAuthDatabasePath()
{
	QDir app_dir( qApp->applicationDirPath() );

	QString start_dir;
	if( !ui.databasePath->text().isEmpty() )
		start_dir = QFileInfo( ui.authPath->text() ).absolutePath();

	QString path = QFileDialog::getOpenFileName(this, tr("Auth Database Path"),
		start_dir, "SQLite3 Database (*.db)");
	if( !path.isEmpty() )
		ui.authPath->setText( app_dir.relativeFilePath(path) );
}

void Settings::browseShoutboxLogPath()
{
	QDir app_dir( qApp->applicationDirPath() );

	QString start_dir;
	if( !ui.shoutboxPath->text().isEmpty() )
		start_dir = QFileInfo( ui.shoutboxPath->text() ).absolutePath();

	QString path = QFileDialog::getSaveFileName(this, tr("Shoutbox Log Path"),
		start_dir, "Shoutbox Log (*.log)");
	if( !path.isEmpty() )
		ui.shoutboxPath->setText( app_dir.relativeFilePath(path) );
}

void Settings::updateConnectionType()
{
	int index = ui.connectionType->currentIndex();

	ui.connectionIP->setEnabled(index == 2);
}

void Settings::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	QLabel *label = qobject_cast<QLabel*>(widget);
	if(label)
		label->setMargin(5);
#ifndef QT_NO_DEBUG
	else
		std::cout << tr("darkenWidget(%1) is not a QLabel").arg(
			widget->objectName()).toLocal8Bit().data() << std::endl;
#endif // QT_NO_DEBUG

	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
}
