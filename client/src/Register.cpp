/******************************************************************************\
*  client/src/Register.cpp                                                     *
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

#include "Register.h"
#include "Settings.h"
#include "Taskbar.h"
#include "Options.h"
#include "registration.h"
#include "Login.h"
#include "sha1.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QMessageBox>

Register::Register(QWidget *parent_widget) : QWidget(parent_widget),
	mFirst(true)
{
	ui.setupUi(this);

	QUrl url = settings->url();
	QString backend_path = QFileInfo( url.path() ).dir().path();
	if(backend_path == "/")
		url.setPath("/register.php");
	else
		url.setPath(backend_path + "/register.php");

	mRegistration = new registration;
	mRegistration->loadValidationImage(url);

	connect(mRegistration, SIGNAL(validationImage(const QPixmap&, const QString&)),
		this, SLOT(validationImage(const QPixmap&, const QString&)));

	connect(ui.existingButton, SIGNAL(clicked(bool)), this, SLOT(showLogin()));
	connect(ui.registerButton, SIGNAL(clicked(bool)), this, SLOT(sendRequest()));

	connect(ui.emailEdit, SIGNAL(returnPressed()), this, SLOT(sendRequest()));
	connect(ui.nameEdit, SIGNAL(returnPressed()), this, SLOT(sendRequest()));
	connect(ui.passwordEdit, SIGNAL(returnPressed()), this, SLOT(sendRequest()));
	connect(ui.passwordEdit2, SIGNAL(returnPressed()), this, SLOT(sendRequest()));
	connect(ui.validationEdit, SIGNAL(returnPressed()), this, SLOT(sendRequest()));
	connect(ui.guestButton, SIGNAL(clicked(bool)), this, SLOT(guestLogin()));
	connect(ui.optionsButton, SIGNAL(clicked(bool)), this, SLOT(showOptions()));

	connect(mRegistration, SIGNAL(registrationComplete()), this,
		SLOT(registrationComplete()));

	darkenWidget(ui.emailLabel);
	darkenWidget(ui.nameLabel);
	darkenWidget(ui.passwordLabel);
	darkenWidget(ui.passwordLabel2);
	darkenWidget(ui.validationImageLabel);
	darkenWidget(ui.validationLabel);

	setWindowTitle( tr("%1 - Registration").arg(
		tr("Absolutely Frosty") ) );

	ui.grid->setEnabled(false);
	ui.guestButton->setEnabled(false);
	ui.existingButton->setEnabled(false);
	ui.registerButton->setEnabled(false);
}

Register::~Register()
{
	delete mRegistration;
}

void Register::registrationComplete()
{
	QString pass = mRegistration->salt() + ui.passwordEdit->text();
	unsigned char checksum[20];

	sha1((uchar*)pass.toAscii().data(), pass.toAscii().size(), checksum);

	pass = QString();
	for(int i = 0; i < 20; i++)
	{
		int byte = (int)checksum[i];
		pass += QString("%1").arg(byte, 2, 16, QLatin1Char('0'));
	}

	settings->setEmail( ui.emailEdit->text() );
	settings->setPass(pass);

	Taskbar::getSingletonPtr()->show();

	deleteLater();
	close();
}

void Register::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	qobject_cast<QLabel*>(widget)->setMargin(5);
	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
}

void Register::validationImage(const QPixmap& image, const QString& error)
{
	if(!mFirst)
	{
		if( error.isEmpty() )
		{
			QMessageBox::critical(this, tr("Registration Error"), tr("Registration "
				"failed. Please check you entered everything correctly and try again."));
		}
		else
		{
			QMessageBox::critical(this, tr("Registration Error"), tr("Registration "
				"failed. Please check you entered everything correctly and try again."
				"\n\nThe error from the server was: %1").arg(error));
		}
	}

	ui.validationImage->setPixmap(image);
	ui.validationEdit->clear();

	mFirst = false;
	ui.grid->setEnabled(true);
	ui.guestButton->setEnabled(true);
	ui.existingButton->setEnabled(true);
	ui.registerButton->setEnabled(true);
}

void Register::sendRequest()
{
	if( mRegistration->salt().isEmpty() )
	{
		QMessageBox::critical(this, tr("Registration Error"), tr("Invalid salt"));

		return;
	}

	QRegExp emailRegExp("^[a-zA-Z][\\w\\.-]*[a-zA-Z0-9]@[a-zA-Z0-9]"
		"[\\w\\.-]*[a-zA-Z0-9]\\.[a-zA-Z][a-zA-Z\\.]*[a-zA-Z]$");

	if( !emailRegExp.exactMatch( ui.emailEdit->text() ) )
	{
		QMessageBox::warning(this, tr("Registration Error"), tr("You must enter a "
			"valid email address."));

		return;
	}

	if( ui.passwordEdit->text() != ui.passwordEdit2->text() )
	{
		QMessageBox::warning(this, tr("Registration Error"), tr("Passwords do not"
		"match."));

		return;
	}

	QString pass = ui.passwordEdit->text();
	QRegExp passMatcher("[a-zA-Z0-9]{6,}");

	if( !passMatcher.exactMatch(pass) )
	{
		QMessageBox::warning(this, tr("Registration Error"), tr("Invalid password. "
			"Passwords can only contain a-z, A-Z, and 0-9 and must be at least 6 "
			"characters long"));

		return;
	}

	ui.grid->setEnabled(false);
	ui.guestButton->setEnabled(false);
	ui.existingButton->setEnabled(false);
	ui.registerButton->setEnabled(false);

	pass = mRegistration->salt() + pass;

	unsigned char checksum[20];
	sha1((uchar*)pass.toAscii().data(), pass.toAscii().size(), checksum);

	pass = QString();
	for(int i = 0; i < 20; i++)
	{
		int byte = (int)checksum[i];
		pass += QString("%1").arg(byte, 2, 16, QLatin1Char('0'));
	}

	QUrl url = settings->url();
	QString backend_path = QFileInfo( url.path() ).dir().path();
	if(backend_path == "/")
		url.setPath("/register.php");
	else
		url.setPath(backend_path + "/register.php");

	mRegistration->registerUser(url,
		ui.emailEdit->text(), ui.nameEdit->text(), pass,
		ui.validationEdit->text());
}

void Register::showLogin()
{
	(new Login)->show();
	deleteLater();
	close();
}

void Register::guestLogin()
{
	Taskbar::getSingletonPtr()->show();

	deleteLater();
	close();
}

void Register::showOptions()
{
	Options::getSingletonPtr()->loadSettings();
}
