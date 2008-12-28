/******************************************************************************\
*  client/src/BindIcon.cpp                                                     *
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

#include "BindIcon.h"
#include "IconEdit.h"
#include "IconSelect.h"
#include "AjaxView.h"

#include <QtCore/QFileInfo>

#include <QtGui/QLabel>

BindIcon::BindIcon(AjaxView *ajax_view, QObject *obj_parent) :
	AjaxBind(obj_parent), mViewer(0), mEditor(0)
{
	mSelector = new IconSelect(ajax_view);
	Q_ASSERT(mSelector);

	connect(mSelector, SIGNAL(iconReady(const QString&, const QString&)),
		this, SLOT(updateIcon(const QString&, const QString&)));
}

QWidget* BindIcon::viewer() const
{
	return mViewer;
}

void BindIcon::setViewer(QWidget *view)
{
	Q_ASSERT( view->inherits("QLabel") );

	mViewer = qobject_cast<QLabel*>(view);
	mViewer->setPixmap( QPixmap(":/blank.png") );
}

QWidget* BindIcon::editor() const
{
	return mEditor;
}

void BindIcon::setEditor(QWidget *edit)
{
	Q_ASSERT( edit->inherits("IconEdit") );

	if(mEditor)
	{
		disconnect(mEditor, SIGNAL(iconDoubleClicked()),
			mSelector, SLOT(selectIcon()));
	}

	mEditor = qobject_cast<IconEdit*>(edit);
	mEditor->setPixmap( QPixmap(":/blank.png") );

	connect(mEditor, SIGNAL(iconDoubleClicked()),
		mSelector, SLOT(selectIcon()));
}

void BindIcon::clear()
{
	Q_ASSERT( mViewer && mEditor );

	mViewer->setPixmap( QPixmap(":/blank.png") );
	mEditor->setPixmap( QPixmap(":/blank.png") );
}

void BindIcon::handleViewResponse(const QVariantMap& values)
{
	Q_ASSERT( mViewer && mEditor );
	Q_ASSERT( values.contains(column()) );

	QVariant data = values.value(column());

	Q_ASSERT( data.canConvert<QString>() );

	QString icon_path = mPath.arg( data.toString() );

	if( QFileInfo(icon_path).exists() )
	{
		mViewer->setPixmap( QPixmap(icon_path) );
		mEditor->setPixmap( QPixmap(icon_path) );
		mEditor->setValue( data.toString() );
	}
	else if( data.toString().isEmpty() )
	{
		mViewer->setPixmap( QPixmap(":/blank.png") );
		mEditor->setPixmap( QPixmap(":/blank.png") );
		mEditor->setValue( QString() );
	}
	else
	{
		mViewer->setPixmap( QPixmap(":/missing.png") );
		mEditor->setPixmap( QPixmap(":/missing.png") );
		mEditor->setValue( data.toString() );
	}
}

void BindIcon::retrieveUpdateData(QVariantMap& row)
{
	Q_ASSERT( mViewer && mEditor );

	row[column()] = mEditor->value();
}

void BindIcon::updateIcon(const QString& icon_path, const QString& value)
{
	Q_ASSERT( mViewer && mEditor );

	mEditor->setValue(value);
	mEditor->setPixmap(icon_path);
}

QString BindIcon::path() const
{
	return mPath;
}

void BindIcon::setPath(const QString& p)
{
	Q_ASSERT( !p.isEmpty() );

	mPath = p;
}

QString BindIcon::searchPath() const
{
	return mSearchPath;
}

void BindIcon::setSearchPath(const QString& p)
{
	Q_ASSERT( !p.isEmpty() );

	mSearchPath = p;
	mSelector->setSearchPath(p);
}
