/******************************************************************************\
*  client/src/BindNumber.cpp                                                   *
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

#include "BindNumber.h"

#include <QtGui/QLabel>
#include <QtGui/QSpinBox>

BindNumber::BindNumber(QObject *obj_parent) : AjaxBind(obj_parent),
	mViewer(0),	mEditor(0), mDefaultValue(-1)
{
	// Nothing to see here
}

QWidget* BindNumber::viewer() const
{
	return mViewer;
}

void BindNumber::setViewer(QWidget *view)
{
	Q_ASSERT( view->inherits("QLabel") );

	mViewer = qobject_cast<QLabel*>(view);
}

QWidget* BindNumber::editor() const
{
	return mEditor;
}

void BindNumber::setEditor(QWidget *edit)
{
	Q_ASSERT( edit->inherits("QSpinBox") );

	mEditor = qobject_cast<QSpinBox*>(edit);
}

void BindNumber::clear()
{
	Q_ASSERT( mViewer && mEditor );

	mViewer->clear();
	mEditor->setValue(mDefaultValue);
}

void BindNumber::handleViewResponse(const QVariantMap& values)
{
	Q_ASSERT( mViewer && mEditor );
	Q_ASSERT( values.contains(column()) );

	QVariant data = values.value(column());

	Q_ASSERT( data.canConvert<int>() );

	int val = data.toInt();

	if(val < 0 )
	{
		mViewer->setText( tr("N/A") );
	}
	else
	{
		if( mPattern.isEmpty() )
			mViewer->setText( QString::number(val) );
		else
			mViewer->setText( mPattern.arg(val) );
	}

	mEditor->setValue(val);
}

void BindNumber::retrieveUpdateData(QVariantMap& row)
{
	Q_ASSERT( mViewer && mEditor );

	row[column()] = mEditor->value();
}

int BindNumber::defaultValue() const
{
	return mDefaultValue;
}

void BindNumber::setDefaultValue(int val)
{
	mDefaultValue = val;
}

QString BindNumber::pattern() const
{
	return mPattern;
}

void BindNumber::setPattern(const QString& p)
{
	mPattern = p;
}
