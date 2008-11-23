/******************************************************************************\
*  client/src/BindNumberSelector.h                                             *
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

#ifndef __BindNumberSelector_h__
#define __BindNumberSelector_h__

#include "AjaxBind.h"

class QLabel;
class QSpinBox;
class QRadioButton;
class QButtonGroup;

typedef QMap<QRadioButton*, QString> RadioButtonMap;

class BindNumberSelector : public AjaxBind
{
	Q_OBJECT

public:
	BindNumberSelector(QObject *parent = 0);

	virtual QWidget* viewer() const;
	virtual void setViewer(QWidget *view);

	virtual QWidget* editor() const;
	virtual void setEditor(QWidget *edit);

	virtual void clear();

	virtual void handleViewResponse(const QVariantMap& values);
	virtual void retrieveUpdateData(QVariantMap& row);

	int defaultValue() const;
	void setDefaultValue(int value);

	RadioButtonMap viewerSelectors() const;
	void setViewerSelectors(const RadioButtonMap& selectors);

	RadioButtonMap editorSelectors() const;
	void setEditorSelectors(const RadioButtonMap& selectors);

	QRadioButton* defaultViewerSelector() const;
	void setDefaultViewerSelector(QRadioButton *defaultSelector);

	QRadioButton* defaultEditorSelector() const;
	void setDefaultEditorSelector(QRadioButton *defaultSelector);

	QButtonGroup* viewerGroup() const;
	void setViewerGroup(QButtonGroup *group);

	QButtonGroup* editorGroup() const;
	void setEditorGroup(QButtonGroup *group);

public slots:
	void updateValue();
	void updateSelection();

protected:
	QLabel *mViewer;
	QSpinBox *mEditor;

	QVariantMap mStaticData, mData;

	QString mViewerCurrent, mEditorCurrent;

	QButtonGroup *mViewerGroup, *mEditorGroup;

	QRadioButton *mDefaultViewerSelector;
	QRadioButton *mDefaultEditorSelector;

	RadioButtonMap mSelectors;
	RadioButtonMap mViewerSelectors;
	RadioButtonMap mEditorSelectors;

	int mDefaultValue;
};

#endif // __BindNumberSelector_h__
