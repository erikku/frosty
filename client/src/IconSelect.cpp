/******************************************************************************\
*  client/src/IconSelect.cpp                                                   *
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

#include "IconSelect.h"

#include <QtCore/QRegExp>
#include <QtCore/QDir>

#include <QtGui/QMessageBox>

IconSelect::IconSelect(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(handleIcon()));
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(handleClose()));

	setWindowModality(Qt::ApplicationModal);
};

void IconSelect::selectIcon(const QString& path)
{
	ui.iconList->clear();

	mIconPath = QString("icons/%1").arg(path);

	QDir dir(mIconPath);
	if( !dir.exists() )
		return;

	QStringList icons = dir.entryList(QStringList() << "icon_*.png",
		QDir::Files | QDir::Readable, QDir::Name);

	QRegExp iconMatcher("icon_(.*).png");

	foreach(QString icon, icons)
	{
		iconMatcher.exactMatch(icon);
		ui.iconList->addItem( new QListWidgetItem(
			QIcon(dir.filePath(icon)), iconMatcher.cap(1) ) );
	}

	show();
};

void IconSelect::handleIcon()
{
	QListWidgetItem *item = ui.iconList->currentItem();
	if( !item )
	{
		QMessageBox::warning(this, tr("Icon Selection Error"),
			tr("You must select an icon first!"));

		return;
	}

	emit iconReady(mIconPath + QString("/icon_") + item->text() + QString(".png"),
		item->text() );

	close();
};

void IconSelect::handleClose()
{
	emit iconCanceled();

	close();
};
