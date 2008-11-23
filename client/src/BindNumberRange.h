/******************************************************************************\
*  client/src/BindNumberRange.h                                                *
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

#ifndef __BindNumberRange_h__
#define __BindNumberRange_h__

#include "AjaxBind.h"

class QLabel;
class QSpinBox;

class BindNumberRange : public AjaxBind
{
	Q_OBJECT

public:
	BindNumberRange(QObject *parent = 0);

	virtual QString column() const;
	virtual void setColumn(const QString& column);

	virtual QStringList columns() const;
	virtual void setColumns(const QStringList& columns);

	virtual QWidget* viewer() const;
	virtual void setViewer(QWidget *view);

	virtual QWidget* editor() const;
	virtual void setEditor(QWidget *edit);

	virtual void clear();

	virtual void handleViewResponse(const QVariantMap& values);
	virtual void retrieveUpdateData(QVariantMap& row);

	QSpinBox* fromEditor() const;
	void setFromEditor(QSpinBox *from);

	QSpinBox* toEditor() const;
	void setToEditor(QSpinBox *to);

	QString fromColumn() const;
	void setFromColumn(const QString& column);

	QString toColumn() const;
	void setToColumn(const QString& column);

	int defaultMin() const;
	void setDefaultMin(int min);

	int defaultMax() const;
	void setDefaultMax(int max);

	bool hideNegative() const;
	void setHideNegative(bool hide);

protected:
	QLabel *mViewer;
	QSpinBox *mFrom, *mTo;

	QString mFromColumn, mToColumn;
	int mDefaultMin, mDefaultMax;
	bool mHideNegative;
};

#endif // __BindNumberRange_h__
