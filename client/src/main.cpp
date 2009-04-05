/******************************************************************************\
*  client/src/main.cpp                                                         *
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

#include <QtGui/QApplication>
#include <QtGui/QPalette>

#include <QtCore/QTranslator>
#include <QtCore/QFile>

#include "PaletteEditor.h"
#include "VersionCheck.h"
#include "IconImport.h"
#include "Settings.h"
#include "Register.h"
#include "Taskbar.h"

#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslCertificate>

int main(int argc, char *argv[])
{
	QApplication::setStyle("plastique");

	QApplication app(argc, argv);

	QApplication::setWindowIcon( QIcon( ":/frosty.png") );

	QTranslator translator;
	translator.load( QString("frosty_%1").arg(settings->lang()) );
	app.installTranslator(&translator);

	QFile paletteFile(":/dark.xml");
	paletteFile.open(QIODevice::ReadOnly);
	QPalette palette = PaletteEditor::importPalette( paletteFile.readAll() );
	paletteFile.close();

	app.setPalette(palette);

	QFile cert_file(":/ca.crt");
    cert_file.open(QIODevice::ReadOnly);

	QSslCertificate cert(&cert_file);
	QSslSocket::addDefaultCaCertificate(cert);

	if( settings->email().isEmpty() )
		(new Register)->show();
	else
		Taskbar::getSingletonPtr()->show();

	if( !app.arguments().contains("--no-check") )
	{
		VersionCheck::getSingletonPtr();

		(new IconImport)->performCheck();
	}

	return app.exec();
}
