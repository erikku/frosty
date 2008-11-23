/******************************************************************************\
*  client/src/BindNumberSet.h                                                  *
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

#ifndef __BindNumberSet_h__
#define __BindNumberSet_h__

#include "AjaxBind.h"

class QLabel;
class QSpinBox;
class QComboBox;

class BindNumberSet : public AjaxBind
{
	Q_OBJECT

public:
	BindNumberSet(QObject *parent = 0);

	virtual void setColumns(const QStringList& columns);

	virtual QWidget* viewer() const;
	virtual void setViewer(QWidget *view);

	virtual QWidget* editor() const;
	virtual void setEditor(QWidget *edit);

	virtual void clear();

	virtual void handleViewResponse(const QVariantMap& values);
	virtual void retrieveUpdateData(QVariantMap& row);

	int defaultValue() const;
	void setDefaultValue(int value);

	QString separator() const;
	void setSeparator(const QString& sep);

	QStringList patterns() const;
	void setPatterns(const QStringList& patterns);

	QComboBox* selector() const;
	void setSelector(QComboBox *selector);

protected slots:
	void updateNumberSetValues();
	void updateNumberSetSelection();

protected:
	QLabel *mViewer;
	QSpinBox *mEditor;
	QComboBox *mSelector;

	QString mCurrent;
	QVariantMap mData;

	QString mSeparator;
	QStringList mPatterns;

	int mDefaultValue;
};

#endif // __BindNumberSet_h__
