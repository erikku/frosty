/******************************************************************************\
*  client/src/AjaxBind.h                                                       *
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

#ifndef __AjaxBind_h__
#define __AjaxBind_h__

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QStringList>

#include <QtGui/QWidget>

class AjaxBind : public QObject
{
	Q_OBJECT

public:
	AjaxBind(QObject *parent = 0);
	virtual ~AjaxBind();

	virtual QString column() const;
	virtual void setColumn(const QString& column);

	virtual QStringList columns() const;
	virtual void setColumns(const QStringList& columns);

	virtual QVariantList customViewActions() const;
	virtual QVariantList customUpdateActions() const;
	virtual QVariantList customDeleteActions() const;

	virtual QWidget* viewer() const = 0;
	virtual void setViewer(QWidget *view) = 0;

	virtual QWidget* editor() const = 0;
	virtual void setEditor(QWidget *edit) = 0;

	virtual void clear() = 0;

	virtual void handleViewResponse(const QVariantMap& values) = 0;
	virtual void retrieveUpdateData(QVariantMap& row) = 0;

protected:
	QStringList mColumns;
};

#endif // __AjaxBind_h__
