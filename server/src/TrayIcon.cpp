/******************************************************************************\
*  server/src/TrayIcon.cpp                                                     *
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

#include "TrayIcon.h"
#include "Settings.h"
#include "Auth.h"

#include "ui_About.h"

#include <QtCore/QFile>

#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QApplication>

TrayIcon::TrayIcon(const QIcon& tray_icon, QObject *parent_object) :
	QSystemTrayIcon(tray_icon, parent_object)
{
	QMenu *menu = new QMenu( tr("Absolutely Frosty Server") );
	QAction *act;

	// About Action
	act = menu->addAction(/* icon, */tr("&About") );
	connect(act, SIGNAL(triggered()), this, SLOT(showAbout()));

	// Settings
	act = menu->addAction(/* icon, */tr("&Config") );
	connect(act, SIGNAL(triggered()), this, SLOT(showSettings()));

	act = menu->addSeparator();

	// Shutdown Action
	act = menu->addAction(/* icon, */tr("&Shutdown") );
	connect(act, SIGNAL(triggered()), this, SLOT(shutdown()));

	setContextMenu(menu);
}

void TrayIcon::shutdown()
{
	hide();

	delete Auth::getSingletonPtr();
	qApp->quit();
}

void TrayIcon::showAbout()
{
	Ui::About about;

	QFile file(":/gpl.txt");
	file.open(QIODevice::ReadOnly);

	QString gpl = QString::fromUtf8( file.readAll() );

	file.close();

	QWidget *widget = new QWidget;
	about.setupUi(widget);
	about.license->setPlainText(gpl);

	widget->show();
}

void TrayIcon::showSettings()
{
	(new Settings)->load();
}
