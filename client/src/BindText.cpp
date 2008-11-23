/******************************************************************************\
*  client/src/BindText.cpp                                                     *
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

#include "BindText.h"

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

BindText::BindText(QObject *obj_parent) : AjaxBind(obj_parent),
	mViewer(0),	mEditor(0)
{
	// Nothing to see here
}

QWidget* BindText::viewer() const
{
	return mViewer;
}

void BindText::setViewer(QWidget *view)
{
	Q_ASSERT( view->inherits("QLabel") );

	mViewer = qobject_cast<QLabel*>(view);
}

QWidget* BindText::editor() const
{
	return mEditor;
}

void BindText::setEditor(QWidget *edit)
{
	Q_ASSERT( edit->inherits("QLineEdit") );

	mEditor = qobject_cast<QLineEdit*>(edit);
}

void BindText::clear()
{
	Q_ASSERT( mViewer && mEditor );

	mViewer->clear();
	mEditor->clear();
}

void BindText::handleViewResponse(const QVariantMap& values)
{
	Q_ASSERT( mViewer && mEditor );
	Q_ASSERT( values.contains(column()) );

	QVariant data = values.value(column());

	Q_ASSERT( data.canConvert<QString>() );

	QString text = data.toString();

	mViewer->setText(text);
	mEditor->setText(text);
}

void BindText::retrieveUpdateData(QVariantMap& row)
{
	Q_ASSERT( mViewer && mEditor );

	row[column()] = mEditor->text();
}
