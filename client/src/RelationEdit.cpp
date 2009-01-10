/******************************************************************************\
*  client/src/RelationEdit.cpp                                                 *
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

#include "RelationEdit.h"
#include "ajax.h"

#include <QtGui/QLabel>

RelationEdit::RelationEdit(QWidget *parent_widget) :
	QDialog(parent_widget), mID(-1)
{
	setWindowModality(Qt::WindowModal);

	ajax::getSingletonPtr()->subscribe(this);
}

void RelationEdit::darkenWidget(QWidget *widget)
{
	QPalette dark_palette = palette();
	dark_palette.setBrush(QPalette::Window,
		dark_palette.brush(QPalette::Base));

	qobject_cast<QLabel*>(widget)->setMargin(5);
	widget->setAutoFillBackground(true);
	widget->setPalette(dark_palette);
}

void RelationEdit::showEdit(const QString& window_title,
	const QString& table, int id)
{
	mID = id;
	mTable = table;

	setWindowTitle(window_title);
	refresh();
	show();
}

void RelationEdit::ajaxResponse(const QVariantMap& resp,
	const QString& user_data)
{
	Q_UNUSED(resp);

	if( mTable.isEmpty() )
		return;

	if(user_data == QString("%1_relation_updated").arg(mTable))
	{
		setWindowModality(Qt::NonModal);
		emit relationsUpdated();

		close();
	}
}
