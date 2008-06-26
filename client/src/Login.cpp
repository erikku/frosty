/******************************************************************************\
*  client/src/Login.cpp                                                        *
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

#include "Login.h"
#include "Settings.h"
#include "SkillWindow.h"
#include "registration.h"
#include "sha1.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QMessageBox>

Login::Login(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	darkenWidget(ui.emailLabel);
	darkenWidget(ui.passwordLabel);

	setWindowTitle( tr("%1 - Login").arg(
		tr("Shin Megami Tensei IMAGINE DB") ) );

	connect(ui.loginButton, SIGNAL(clicked(bool)), this, SLOT(login()));
	connect(ui.emailEdit, SIGNAL(returnPressed()), this, SLOT(login()));
	connect(ui.passwordEdit, SIGNAL(returnPressed()), this, SLOT(login()));

	mRegistration = 0;
};

void Login::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	qobject_cast<QLabel*>(widget)->setMargin(5);
	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
};

void Login::login()
{
	QRegExp emailRegExp("^[a-zA-Z][\\w\\.-]*[a-zA-Z0-9]@[a-zA-Z0-9]"
		"[\\w\\.-]*[a-zA-Z0-9]\\.[a-zA-Z][a-zA-Z\\.]*[a-zA-Z]$");

	if( !emailRegExp.exactMatch( ui.emailEdit->text() ) )
	{
		QMessageBox::warning(this, tr("Login Error"), tr("You must enter a "
			"valid email address."));

		return;
	}

	QString pass = ui.passwordEdit->text();
	QRegExp passMatcher("[a-zA-Z0-9]{6,}");

	if( !passMatcher.exactMatch(pass) )
	{
		QMessageBox::warning(this, tr("Login Error"), tr("Invalid password. "
			"Passwords can only contain a-z, A-Z, and 0-9 and must be at least 6 "
			"characters long"));

		return;
	}

	QUrl url = settings->url();
	url.setPath(QFileInfo( url.path() ).dir().path() + "/register.php");

	mRegistration = new registration;
	mRegistration->loadValidationImage(url);

	connect(mRegistration, SIGNAL(validationImage(const QPixmap&, const QString&)),
		this, SLOT(validationImage(const QPixmap&, const QString&)));

	setEnabled(false);
};

void Login::validationImage(const QPixmap& image, const QString& error)
{
	Q_UNUSED(image);
	Q_UNUSED(error);

	if( mRegistration->salt().isEmpty() )
	{
		QMessageBox::critical(this, tr("Login Error"), tr("Invalid salt"));

		return;
	}

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

	(new SkillWindow)->show();

	mRegistration->deleteLater();
	deleteLater();
	close();
};
