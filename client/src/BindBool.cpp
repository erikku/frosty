/******************************************************************************\
*  client/src/BindBool.cpp                                                     *
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

#include "BindBool.h"

#include <QtGui/QLabel>
#include <QtGui/QCheckBox>

BindBool::BindBool(QObject *obj_parent) : AjaxBind(obj_parent),
	mViewer(0),	mEditor(0)
{
	// Nothing to see here
}

QWidget* BindBool::viewer() const
{
	return mViewer;
}

void BindBool::setViewer(QWidget *view)
{
	Q_ASSERT( view->inherits("QLabel") );

	mViewer = qobject_cast<QLabel*>(view);
}

QWidget* BindBool::editor() const
{
	return mEditor;
}

void BindBool::setEditor(QWidget *edit)
{
	Q_ASSERT( edit->inherits("QCheckBox") );

	mEditor = qobject_cast<QCheckBox*>(edit);
}

void BindBool::clear()
{
	Q_ASSERT( mViewer && mEditor );

	mViewer->clear();
	mEditor->setCheckState(Qt::Unchecked);
}

void BindBool::handleViewResponse(const QVariantMap& values)
{
	Q_ASSERT( mViewer && mEditor );
	Q_ASSERT( values.contains(column()) );

	QVariant data = values.value(column());

	Q_ASSERT( data.canConvert<bool>() );

	bool cond = data.toBool();

	QString yes = mYesText.isEmpty() ? tr("x") : mYesText;
	QString no = mNoText.isEmpty() ? tr("-") : mNoText;

	mViewer->setText(cond ? yes : no);
	mEditor->setCheckState(cond ? Qt::Checked : Qt::Unchecked);
}

void BindBool::retrieveUpdateData(QVariantMap& row)
{
	Q_ASSERT( mViewer && mEditor );

	row[column()] = mEditor->checkState() == Qt::Checked ? true : false;
}

QString BindBool::yesText() const
{
	return mYesText;
}

void BindBool::setYesText(const QString& yes)
{
	mYesText = yes;
}

QString BindBool::noText() const
{
	return mNoText;
}

void BindBool::setNoText(const QString& no)
{
	mNoText = no;
}
