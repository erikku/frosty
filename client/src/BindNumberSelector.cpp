/******************************************************************************\
*  client/src/BindNumberSelector.cpp                                           *
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

#include "BindNumberSelector.h"

#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QRadioButton>
#include <QtGui/QButtonGroup>

BindNumberSelector::BindNumberSelector(QObject *obj_parent) : AjaxBind(obj_parent),
	mViewer(0),	mEditor(0), mViewerGroup(0), mEditorGroup(0),
	mDefaultViewerSelector(0), mDefaultEditorSelector(0), mDefaultValue(0)
{
	// Nothing to see here
}

QWidget* BindNumberSelector::viewer() const
{
	return mViewer;
}

void BindNumberSelector::setViewer(QWidget *view)
{
	Q_ASSERT( view->inherits("QLabel") );

	mViewer = qobject_cast<QLabel*>(view);
}

QWidget* BindNumberSelector::editor() const
{
	return mEditor;
}

void BindNumberSelector::setEditor(QWidget *edit)
{
	Q_ASSERT( edit->inherits("QSpinBox") );

	if(mEditor)
	{
		disconnect(mEditor, SIGNAL(valueChanged(int)),
			this, SLOT(updateNumberSelectionValue()));
	}

	mEditor = qobject_cast<QSpinBox*>(edit);

	connect(mEditor, SIGNAL(valueChanged(int)),
		this, SLOT(updateValue()));
}

void BindNumberSelector::clear()
{
	Q_ASSERT( mViewer && mEditor );

	bool block = mEditor->blockSignals(true);
	mEditor->setValue(mDefaultValue);
	mEditor->blockSignals(block);

	mData.clear();
	foreach(QString col, mColumns)
		mData[col] = mDefaultValue;

	mStaticData = mData;

	mViewer->clear();
}

void BindNumberSelector::handleViewResponse(const QVariantMap& values)
{
	Q_ASSERT( mViewer && mEditor );

	QStringList final;

	for(int i = 0; i < mColumns.count(); i++)
	{
		QString col = mColumns.at(i);

		Q_ASSERT( values.contains(col) );

		QVariant data = values.value(col);

		Q_ASSERT( data.canConvert<int>() );

		int number = data.toInt();

		// Save the number
		mData[col] = number;


	}

	mStaticData = mData;

	mViewer->setText( QString::number( mData.value(mViewerCurrent).toInt() ) );

	bool block = mEditor->blockSignals(true);
	mEditor->setValue( mData.value(mEditorCurrent).toInt() );
	mEditor->blockSignals(block);
}

void BindNumberSelector::retrieveUpdateData(QVariantMap& row)
{
	Q_ASSERT( mViewer && mEditor && mData.count() );

	row.unite(mData);
}

int BindNumberSelector::defaultValue() const
{
	return mDefaultValue;
}

void BindNumberSelector::setDefaultValue(int value)
{
	mDefaultValue = value;
}

RadioButtonMap BindNumberSelector::viewerSelectors() const
{
	return mViewerSelectors;
}

void BindNumberSelector::setViewerSelectors(const RadioButtonMap& selectors)
{
	Q_ASSERT( mColumns.count() );

	// TODO: Clean up the old ones if they exist

	mViewerSelectors = selectors;

	if( mViewerCurrent.isEmpty() )
		mViewerCurrent = mColumns.first();

	mSelectors = selectors;
	mSelectors.unite(mEditorSelectors);
}

RadioButtonMap BindNumberSelector::editorSelectors() const
{
	return mEditorSelectors;
}

void BindNumberSelector::setEditorSelectors(const RadioButtonMap& selectors)
{
	Q_ASSERT( mColumns.count() );

	// TODO: Clean up the old ones if they exist

	mEditorSelectors = selectors;

	if( mEditorCurrent.isEmpty() )
		mEditorCurrent = mColumns.first();

	mSelectors = selectors;
	mSelectors.unite(mViewerSelectors);
}

QRadioButton* BindNumberSelector::defaultViewerSelector() const
{
	return mDefaultViewerSelector;
}

void BindNumberSelector::setDefaultViewerSelector(QRadioButton *defaultSelector)
{
	Q_ASSERT( mColumns.count() );
	Q_ASSERT( mViewerSelectors.count() );

	if(!defaultSelector)
	{
		mViewerCurrent = mColumns.first();
		mDefaultViewerSelector = 0;

		return;
	}

	mDefaultViewerSelector = defaultSelector;
	mViewerCurrent = mViewerSelectors.value(defaultSelector);

	if(!mViewerGroup)
	{
		mViewerGroup = new QButtonGroup;

		QList<QRadioButton*> buttons = mViewerSelectors.keys();
		foreach(QRadioButton *button, buttons)
			mViewerGroup->addButton(button);

		connect(mViewerGroup, SIGNAL(buttonClicked(QAbstractButton*)),
			this, SLOT(updateSelection()));
	}

	defaultSelector->setChecked(true);
}

QRadioButton* BindNumberSelector::defaultEditorSelector() const
{
	return mDefaultEditorSelector;
}

void BindNumberSelector::setDefaultEditorSelector(QRadioButton *defaultSelector)
{
	Q_ASSERT( mColumns.count() );
	Q_ASSERT( mViewerSelectors.count() );

	if(!defaultSelector)
	{
		mEditorCurrent = mColumns.first();
		mDefaultEditorSelector = 0;

		return;
	}

	mDefaultEditorSelector = defaultSelector;
	mEditorCurrent = mEditorSelectors.value(defaultSelector);

	if(!mEditorGroup)
	{
		mEditorGroup = new QButtonGroup;

		QList<QRadioButton*> buttons = mEditorSelectors.keys();
		foreach(QRadioButton *button, buttons)
			mEditorGroup->addButton(button);

		connect(mEditorGroup, SIGNAL(buttonClicked(QAbstractButton*)),
			this, SLOT(updateSelection()));
	}

	defaultSelector->setChecked(true);
}

void BindNumberSelector::updateSelection()
{
	QButtonGroup *group = qobject_cast<QButtonGroup*>( sender() );
	Q_ASSERT(group);

	QRadioButton *radio = qobject_cast<QRadioButton*>( group->checkedButton() );
	Q_ASSERT(radio);

	if(mViewerGroup == group)
	{
		QString col = mViewerSelectors.value(radio);

		mViewer->setText( QString::number( mStaticData.value(col).toInt() ) );

		mViewerCurrent = col;
	}
	else if(mEditorGroup == group)
	{
		QString col = mEditorSelectors.value(radio);

		bool block = mEditor->blockSignals(true);
		mEditor->setValue( mData.value(col).toInt() );
		mEditor->blockSignals(block);

		mEditorCurrent = col;
	}
}

void BindNumberSelector::updateValue()
{
	Q_ASSERT( !mEditorCurrent.isEmpty() );
	Q_ASSERT( mData.contains(mEditorCurrent) );

	mData[mEditorCurrent] = mEditor->value();
}

QButtonGroup* BindNumberSelector::viewerGroup() const
{
	return mViewerGroup;
}

void BindNumberSelector::setViewerGroup(QButtonGroup *group)
{
	if(!group)
		return;

	if(mViewerGroup)
		delete mViewerGroup;

	mViewerGroup = group;

	connect(mViewerGroup, SIGNAL(buttonClicked(QAbstractButton*)),
		this, SLOT(updateSelection()));
}

QButtonGroup* BindNumberSelector::editorGroup() const
{
	return mEditorGroup;
}

void BindNumberSelector::setEditorGroup(QButtonGroup *group)
{
	if(!group)
		return;

	if(mEditorGroup)
		delete mEditorGroup;

	mEditorGroup = group;

	connect(mEditorGroup, SIGNAL(buttonClicked(QAbstractButton*)),
		this, SLOT(updateSelection()));
}
