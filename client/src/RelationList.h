/******************************************************************************\
*  client/src/RelationList.h                                                   *
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

#ifndef __RelationList_h__
#define __RelationList_h__

#include "ui_RelationList.h"

class RelationEdit;

class RelationList : public QWidget
{
	Q_OBJECT

public:
	RelationList(QWidget *parent = 0);
	~RelationList();

public slots:
	void refresh();
	void showList(const QString& windowTitle, const QString& table);

	void addRelation();
	void editRelation();
	void deleteRelation();

signals:
	void relationsUpdated();

protected slots:
	void itemSelectionChanged();
	void ajaxResponse(const QVariant& resp);

protected:
	QString mTable;
	RelationEdit *mEdit;

	Ui::RelationList ui;
};

#endif // __RelationList_h__
