/******************************************************************************\
*  client/src/BindNumberSet.cpp                                                *
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

#include "BindNumberSet.h"

#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>

BindNumberSet::BindNumberSet(QObject *obj_parent) : AjaxBind(obj_parent),
	mViewer(0),	mEditor(0), mDefaultValue(0)
{
	// Nothing to see here
}

void BindNumberSet::setColumns(const QStringList& cols)
{
	AjaxBind::setColumns(cols);

	foreach(QString col, cols)
		mData[col] = mDefaultValue;
}

QWidget* BindNumberSet::viewer() const
{
	return mViewer;
}

void BindNumberSet::setViewer(QWidget *view)
{
	Q_ASSERT( view->inherits("QLabel") );

	mViewer = qobject_cast<QLabel*>(view);
}

QWidget* BindNumberSet::editor() const
{
	return mEditor;
}

void BindNumberSet::setEditor(QWidget *edit)
{
	Q_ASSERT( edit->inherits("QSpinBox") );

	if(mEditor)
	{
		disconnect(mEditor, SIGNAL(valueChanged(int)),
			this, SLOT(updateNumberSetValues()));
	}

	mEditor = qobject_cast<QSpinBox*>(edit);

	connect(mEditor, SIGNAL(valueChanged(int)),
		this, SLOT(updateNumberSetValues()));
}

void BindNumberSet::clear()
{
	Q_ASSERT( mViewer && mEditor && mSelector );

	mViewer->clear();

	bool block = mEditor->blockSignals(true);
	mEditor->setValue(mDefaultValue);
	mEditor->blockSignals(block);

	QMutableMapIterator<QString, QVariant> it(mData);

	while( it.hasNext() )
	{
		it.next();
		it.setValue(mDefaultValue);
	}

	// Set it back to the first number in the set
	// mSelector->setCurrentIndex(0);
}

void BindNumberSet::handleViewResponse(const QVariantMap& values)
{
	Q_ASSERT( mViewer && mEditor && mSelector );

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

		// If we have a number greater then 0, add it to the view
		if(number > 0)
			final << mPatterns.at(i).arg( QString::number(number) );
	}

	if( final.isEmpty() )
		final << tr("N/A");

	mViewer->setText( final.join(mSeparator) );

	bool block = mEditor->blockSignals(true);
	mEditor->setValue( mData.value(mCurrent).toInt() );
	mEditor->blockSignals(block);

	for(int i = 0; i < mColumns.count(); i++)
	{
		QString col = mColumns.at(i);

		if(values.value(col).toInt() > 0)
		{
			mSelector->setCurrentIndex(i);
			break;
		}
	}
}

void BindNumberSet::retrieveUpdateData(QVariantMap& row)
{
	Q_ASSERT( mViewer && mEditor && mSelector );

	row.unite(mData);
}

int BindNumberSet::defaultValue() const
{
	return mDefaultValue;
}

void BindNumberSet::setDefaultValue(int value)
{
	mDefaultValue = value;

	QMutableMapIterator<QString, QVariant> it(mData);

	while( it.hasNext() )
	{
		it.next();
		it.setValue(mDefaultValue);
	}
}

QString BindNumberSet::separator() const
{
	return mSeparator;
}

void BindNumberSet::setSeparator(const QString& sep)
{
	mSeparator = sep;
}

QStringList BindNumberSet::patterns() const
{
	return mPatterns;
}

void BindNumberSet::setPatterns(const QStringList& p)
{
	mPatterns = p;
}

QComboBox* BindNumberSet::selector() const
{
	return mSelector;
}

void BindNumberSet::setSelector(QComboBox *sel)
{
	if(mSelector)
	{
		disconnect(mSelector, SIGNAL(currentIndexChanged(int)),
			this, SLOT(updateNumberSetSelection()));
	}

	mSelector = sel;

	connect(mSelector, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateNumberSetSelection()));
}

void BindNumberSet::updateNumberSetValues()
{
	Q_ASSERT( mViewer && mEditor && mSelector );

	mData[mCurrent] = mEditor->value();
}

void BindNumberSet::updateNumberSetSelection()
{
	Q_ASSERT( mViewer && mEditor && mSelector );

	int index = mSelector->currentIndex();

	Q_ASSERT( index < mData.count() );
	Q_ASSERT( index < mColumns.count() );

	mCurrent = mColumns.at(index);

	bool block = mEditor->blockSignals(true);
	mEditor->setValue( mData.value(mCurrent).toInt() );
	mEditor->blockSignals(block);
}
