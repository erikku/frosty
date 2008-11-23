/******************************************************************************\
*  client/src/BindIcon.h                                                       *
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

#ifndef __BindIcon_h__
#define __BindIcon_h__

#include "AjaxBind.h"

class QLabel;
class IconEdit;
class IconSelect;

class BindIcon : public AjaxBind
{
	Q_OBJECT

public:
	BindIcon(QObject *parent = 0);

	virtual QWidget* viewer() const;
	virtual void setViewer(QWidget *view);

	virtual QWidget* editor() const;
	virtual void setEditor(QWidget *edit);

	virtual void clear();

	virtual void handleViewResponse(const QVariantMap& values);
	virtual void retrieveUpdateData(QVariantMap& row);

	QString path() const;
	void setPath(const QString& path);

	QString searchPath() const;
	void setSearchPath(const QString& path);

protected slots:
	void updateIcon(const QString& path, const QString& value);

protected:
	QLabel *mViewer;
	IconEdit *mEditor;
	IconSelect *mSelector;

	QString mPath, mSearchPath;
};

#endif // __BindIcon_h__
