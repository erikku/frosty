/******************************************************************************\
*  client/src/BindNumberRange.cpp                                              *
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

#include "BindNumberRange.h"

#include <QtGui/QLabel>
#include <QtGui/QSpinBox>

BindNumberRange::BindNumberRange(QObject *obj_parent) : AjaxBind(obj_parent),
	mViewer(0),	mFrom(0), mTo(0), mDefaultMin(0), mDefaultMax(100),
	mHideNegative(false)
{
	mColumns << QString() << QString();
}

QString BindNumberRange::column() const
{
	Q_ASSERT(!"BindNumberRange::column() is not used!");

	return QString();
}

void BindNumberRange::setColumn(const QString& col)
{
	Q_UNUSED(col);

	Q_ASSERT(!"BindNumberRange::setColumn() will be ignored!");
}

QStringList BindNumberRange::columns() const
{
	Q_ASSERT( !mFromColumn.isEmpty() && !mToColumn.isEmpty() );

	return QStringList() << mFromColumn << mToColumn;
}

void BindNumberRange::setColumns(const QStringList& cols)
{
	Q_ASSERT( cols.count() == 2 );
	Q_ASSERT( !cols.at(0).isEmpty() );
	Q_ASSERT( !cols.at(1).isEmpty() );
	Q_ASSERT( cols.at(0) != cols.at(1) );

	mFromColumn = cols.at(0);
	mToColumn = cols.at(1);
}

QWidget* BindNumberRange::viewer() const
{
	return mViewer;
}

void BindNumberRange::setViewer(QWidget *view)
{
	Q_ASSERT( view->inherits("QLabel") );

	mViewer = qobject_cast<QLabel*>(view);
}

QWidget* BindNumberRange::editor() const
{
	return 0;
}

void BindNumberRange::setEditor(QWidget *edit)
{
	Q_UNUSED(edit);

	Q_ASSERT(!"BindNumberRange::setEditor() is not used!");
}

void BindNumberRange::clear()
{
	Q_ASSERT( mViewer && mFrom && mTo );

	mViewer->clear();

	mFrom->setValue(mDefaultMin);
	mTo->setValue(mDefaultMin);
}

void BindNumberRange::handleViewResponse(const QVariantMap& values)
{
	Q_ASSERT( mViewer && mFrom && mTo );
	Q_ASSERT( values.contains(mFromColumn) );
	Q_ASSERT( values.contains(mToColumn) );

	QVariant from_data = values.value(mFromColumn);
	QVariant to_data = values.value(mToColumn);

	Q_ASSERT( from_data.canConvert<int>() );
	Q_ASSERT( to_data.canConvert<int>() );

	int from = from_data.toInt();
	int to = to_data.toInt();

	QString text;
	if(mHideNegative)
	{
		if(from < 0 && to < 0)
			text = tr("-");
		else if(from < 0)
			text = tr("- %1").arg(to);
		else if(to < 0)
			text = tr("%1 -").arg(from);
		else
			text = tr("%1 - %2").arg(from).arg(to);
	}
	else
	{
		text = tr("%1 - %2").arg(from).arg(to);
	}

	mViewer->setText(text);
	mFrom->setValue(from);
	mTo->setValue(to);
}

void BindNumberRange::retrieveUpdateData(QVariantMap& row)
{
	Q_ASSERT( mViewer && mFrom && mTo );

	row[mFromColumn] = mFrom->value();
	row[mToColumn] = mTo->value();
}

QSpinBox* BindNumberRange::fromEditor() const
{
	return mFrom;
}

void BindNumberRange::setFromEditor(QSpinBox *from)
{
	mFrom = from;
}

QSpinBox* BindNumberRange::toEditor() const
{
	return mTo;
}

void BindNumberRange::setToEditor(QSpinBox *to)
{
	mTo = to;
}

QString BindNumberRange::fromColumn() const
{
	return mFromColumn;
}

void BindNumberRange::setFromColumn(const QString& col)
{
	Q_ASSERT( !col.isEmpty() );

	mFromColumn = col;
	mColumns[0] = col;
}

QString BindNumberRange::toColumn() const
{
	return mToColumn;
}

void BindNumberRange::setToColumn(const QString& col)
{
	Q_ASSERT( !col.isEmpty() );

	mToColumn = col;
	mColumns[1] = col;
}

int BindNumberRange::defaultMin() const
{
	return mDefaultMin;
}

void BindNumberRange::setDefaultMin(int min)
{
	mDefaultMin = min;
}

int BindNumberRange::defaultMax() const
{
	return mDefaultMax;
}

void BindNumberRange::setDefaultMax(int max)
{
	mDefaultMax = max;
}

bool BindNumberRange::hideNegative() const
{
	return mHideNegative;
}

void BindNumberRange::setHideNegative(bool hide)
{
	mHideNegative = hide;
}
