/******************************************************************************\
*  client/src/AjaxBind.cpp                                                     *
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

#include "AjaxBind.h"

AjaxBind::AjaxBind(QObject *obj_parent) : QObject(obj_parent)
{
	// Nothing to see here
}

AjaxBind::~AjaxBind()
{
	// Nothing to see here
}

QString AjaxBind::column() const
{
	Q_ASSERT(mColumns.count() == 1);

	return mColumns.first();
}

void AjaxBind::setColumn(const QString& col)
{
	Q_ASSERT(mColumns.count() <= 1);
	Q_ASSERT( !col.isEmpty() );

	mColumns = QStringList() << col;
}

QStringList AjaxBind::columns() const
{
	return mColumns;
}

void AjaxBind::setColumns(const QStringList& cols)
{
	mColumns = cols;
}

QVariantList AjaxBind::customViewActions() const
{
	return QVariantList();
}

QVariantList AjaxBind::customUpdateActions() const
{
	return QVariantList();
}

QVariantList AjaxBind::customDeleteActions() const
{
	return QVariantList();
}
