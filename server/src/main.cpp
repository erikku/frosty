/******************************************************************************\
*  server/src/main.cpp                                                         *
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

#include "Server.h"

#ifdef QT_GUI_LIB
#include <QtGui/QPalette>

//#include <QtCore/QTranslator>
#include <QtCore/QFile>

#include "PaletteEditor.h"
#endif // QT_GUI_LIB

int main(int argc, char *argv[])
{
#ifdef QT_GUI_LIB
	QApplication::setStyle("plastique");
#endif // QT_GUI_LIB

	Server app(argc, argv);

#ifdef QT_GUI_LIB
	/*
	QTranslator translator;
	translator.load( QString("frosty_%1").arg(settings->lang()) );
	app.installTranslator(&translator);
	*/

	QFile paletteFile(":/dark.xml");
	paletteFile.open(QIODevice::ReadOnly);
	QPalette palette = PaletteEditor::importPalette( paletteFile.readAll() );
	paletteFile.close();

	app.setPalette(palette);
#endif // QT_GUI_LIB

	app.init();

	return app.exec();
}
